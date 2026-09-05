#include "Json.h"

#include <algorithm>
#include <memory>
#include <utility>

Json::Json() : parent(nullptr)
{

}

Json::Json(const std::string& key, const std::string& value) : key(key), value(value), parent(nullptr)
{

}

void Json::SetParent(Json* parent)
{
	this->parent = parent;
}

void Json::RemoveParent()
{
	parent = nullptr;
}

void Json::AddChild(std::unique_ptr<Json> child)
{
	if (child == nullptr)
		return;

	child->SetParent(this);
	this->children.push_back(std::move(child));
}

void Json::RemoveChild(Json* child)
{
	auto iter = std::remove_if
	(
		this->children.begin(),
		this->children.end(),
		[child](const std::unique_ptr<Json>& ptr) { return ptr.get() == child; }
	);

	if (iter != this->children.end())
		this->children.erase(iter, this->children.end());
}

void Json::RemoveChild(int index)
{
	if (index < 0 || index >= static_cast<int>(children.size()))
		return;

	this->children.erase(this->children.begin() + index);
}

std::string Json::ToString(const Json* root)
{
	if (root == nullptr)
		return "";

	std::string result;

	if (!root->key.empty())
		result += "\"" + root->key + "\":";

	if (!root->children.empty() || root->value.empty())
	{
		result += "{";

		for (size_t i = 0; i < root->children.size(); ++i)
		{
			result += ToString(root->children[i].get());

			if (i < root->children.size() - 1)
				result += ",";
		}

		result += "}";
	}

	else
		result += "\"" + root->value + "\"";

	return result;
}

void Json::Destroy(Json* root)
{
	// unique_ptr가 자식 노드들을 관리하므로, 최상위 root만 delete하면 하위 트리가 모두 연쇄적으로 누수 없이 정상 해제
	if (root != nullptr)
		delete root;
}