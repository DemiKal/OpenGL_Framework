#pragma once
class UserInterface
{
public:
	UserInterface() {};
	void Update();
	void Draw();
private:
	void MainMenu();		//menu bar on top
	void EntityBrowser();	//vertical bar to the left
	void EntityInspector();		//vertical bar to the right
};

