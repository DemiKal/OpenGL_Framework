#pragma once
class UserInterface
{
public:
	UserInterface() {};
	void Update(float dt);
	static void Draw();
private:
	void MainMenu(float dt) const;
	void EntityBrowser() const;	//vertical bar to the left
	static void EntityInspector();		//vertical bar to the right
};

