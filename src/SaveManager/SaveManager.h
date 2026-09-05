//
// Created by miocasa on 9/1/26.
//

#ifndef EINK_SAVEMANAGER_H
#define EINK_SAVEMANAGER_H
#include <memory>
#include <vector>
#include <InternalFileSystem.h>

#include "Results.h"
#include "Widgets/BackgroundImage.h"
#include "Widgets/FontManager.h"
#include "Widgets/TextBox.h"
#include "Widgets/Widget.h"
#include "Widgets/widget_t.h"

#define PATH_MAX_LENGTH 256
#define WIDGET_FILE_FORMAT ("%s/widget_%d.bin")
#define BACKGROUND_FILE_FORMAT ("%s/background.bin")
#define SAVEMANAGER_VERSION (1.0)

class SaveManager
{
public:
	SaveManager();
	~SaveManager() = default;
	bool begin(bool format_on_fail = false);
	RESULTS saveWidgets(const char* path, std::vector<std::shared_ptr<Widget>>* widgets);
	RESULTS loadWidgets(const char* path, std::vector<std::shared_ptr<Widget>>* widgets);

	RESULTS saveBackground(const char* path, std::shared_ptr<BackgroundImage>* bg_img);
	RESULTS loadBackground(const char* path, std::shared_ptr<BackgroundImage>* bg_img);

private:
	char _path_buffer[PATH_MAX_LENGTH];
	size_t _size;
	Adafruit_LittleFS_Namespace::File file;

	void create_path(const char* path, uint8_t num);
	RESULTS saveWidget(const char* path, std::shared_ptr<Widget>* widget);
	RESULTS loadWidget(const char* path, std::vector<std::shared_ptr<Widget>>* widgets);

	// widget specific methods
	RESULTS saveTextBox(const char* path, std::shared_ptr<Widget>* widget);
	RESULTS loadTextBox(const char* path, std::vector<std::shared_ptr<Widget>>* widgets, widget_t* header);
};


#endif //EINK_SAVEMANAGER_H
