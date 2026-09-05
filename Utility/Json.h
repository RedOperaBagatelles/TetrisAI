#pragma once

#include <vector>
#include <string>
#include <memory>

class Json
{
public:
	Json();
	Json(const std::string& key, const std::string& value);

	void SetParent(Json* parent);
	void AddChild(std::unique_ptr<Json> child);

	void RemoveParent();
	void RemoveChild(Json* child);
	void RemoveChild(int index);

	std::string key;
	std::string value;

	static std::string ToString(const Json* root);
	static void Destroy(Json* root);

private:
	std::vector<std::unique_ptr<Json>> children;
	Json* parent;
};