#pragma once

#include "../Manager.h"

class InputManager : public Manager{
public:
	static InputManager& GetInstance() {
		static InputManager instance;
		return instance;
	}
	~InputManager();
	void Update() override;
private:
	InputManager() {}
	InputManager(InputManager const&) = delete;
	void operator=(InputManager const&) = delete;
};