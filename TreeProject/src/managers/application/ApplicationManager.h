#pragma once

#include "../Manager.h"

class ApplicationManager : public Manager{
public:
	static ApplicationManager& GetInstance() {
		static ApplicationManager instance;
		return instance;
	}
	~ApplicationManager();
	void Update() override;
private:
	ApplicationManager() {}
	ApplicationManager(ApplicationManager const&) = delete;
	void operator=(ApplicationManager const&) = delete;
};