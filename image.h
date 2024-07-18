#pragma once

class ImageManager
{
	
public:
	map<string, PIMAGE> images;

	void Dispose()
	{
		static bool disposed{ false };
		if (disposed)
			return;

		disposed = true;
		for (auto& pr : images)
			delimage(pr.second);
		
		images.clear();
	}

	void Load(string_view name)
	{	//name includes suffix
		string path = g.image_dir + str(name);
		//DebugLog(path);
		if (!ExistFile(path))
		{
			ErrorLogTip(nullptr, "Error when loading Image: Cannot find " + path, "TerraSurvivor::Load");
			return;
		}
		PIMAGE pimg = newimage();
		getimage(pimg, path.c_str());
		images.insert(make_pair(name, pimg));
	}
	void Load(initializer_list<string> names)
	{
		for (const auto& name : names)
			Load(name);
	}
	inline bool ExistImage(string_view name) const
	{
		return images.find(str(name)) != images.end();
	}
	void Add(PIMAGE img, string_view name)
	{
		images.insert(make_pair(str(name), img));
	}
	inline size_t Size() const
	{
		return images.size();
	}
}imgMgr;