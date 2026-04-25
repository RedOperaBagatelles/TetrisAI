#pragma once

class GameLoop
{
public:
	GameLoop() = default;

	virtual void Initialize() { }
	virtual void BeforeFrame() {}
	virtual void Start() { }
	virtual void LateStart() { }
	virtual void Update(float deltaTime) { }
	virtual bool Render() { return true; }
	virtual void LateUpdate() { }
	virtual void Finalize() { }
	virtual void Destroy() { }

	virtual void OnEnable() { }
	virtual void OnDisable() { }

	virtual void Reset() { }

	virtual ~GameLoop() = default;
};