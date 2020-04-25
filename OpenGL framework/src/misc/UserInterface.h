#pragma once
class UserInterface
{
public:
	UserInterface() {};
	void Update();
	static void Draw();
private:
	void MainMenu() const;		//menu bar on top
	void EntityBrowser() const;	//vertical bar to the left
	static void EntityInspector();		//vertical bar to the right
};

