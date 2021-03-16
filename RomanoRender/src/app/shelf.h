#pragma once

#include "imgui.h"
#include "scene/scene.h"
#include "shading//light.h"
#include "scene/camera.h"
#include "scene/settings.h"
#include "filedialog.h"

#ifndef SHELF
#define SHELF


struct Shelf
{
	Shelf() {}

	~Shelf() {}

	virtual void draw(Render_Settings& settings, 
					  std::vector<Object>& objects, 
					  std::vector<Light*>& lights, 
					  std::vector<Camera>& cameras, Console& console, bool& change, int& edit);
};


struct Geometry_Shelf : public Shelf
{
	Geometry_Shelf() {}

	void draw(Render_Settings& settings,
			  std::vector<Object>& objects,
			  std::vector<Light*>& lights, 
			  std::vector<Camera>& cameras, Console& console, bool& change, int& edit) override;
};


struct Light_Shelf : public Shelf
{
	Light_Shelf() {}

	void draw(Render_Settings& settings,
			  std::vector<Object>& objects, 
			  std::vector<Light*>& lights, 
			  std::vector<Camera>& cameras, Console& console, bool& change, int& edit) override;
};


struct Camera_Shelf : public Shelf
{
	Camera_Shelf() {}

	void draw(Render_Settings& settings,
			  std::vector<Object>& objects,
			  std::vector<Light*>& lights, 
			  std::vector<Camera>& cameras, Console& console, bool& change, int& edit) override;
};

#endif // !SHELF
