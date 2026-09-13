#include "SaveManager.h"

#include "Externs.h"
#include "image_2bpp.h"
#include "Widgets/TextBox.h"


SaveManager::SaveManager()
	: _path_buffer(), _size(0), _flashTransport(), _externalFlash(&_flashTransport), qspiFS(&_externalFlash),
	  file(qspiFS)
{
	_path_buffer[0] = '\0';
}

bool SaveManager::begin(bool format_on_fail)
{
	// memset(&settings, 0, sizeof(settings));
	constexpr settings_t _settings = settings_t{
		SEC_TO_MS(10),
		SEC_TO_MS(30),
		false,
		false,
		{},
		"",
	};
	settings = _settings;

	if (qspiFS.begin(format_on_fail)) return true;
	return false;
}

RESULTS SaveManager::loadConfig(std::vector<std::shared_ptr<Widget>>* widgets, BackgroundImage* bg_img)
{
	auto _settings = settings_t{};
	file = qspiFS.open(CONFIG_PATH, Adafruit_LittleFS_Namespace::FILE_O_READ);

	if (!file) return RESULTS::FILE_OPEN_ERROR;
	size_t read = file.read(&_settings, sizeof(_settings));
	file.close();

	if (read != sizeof(_settings)) return RESULTS::FILE_READ_ERROR;

	settings = _settings;

	if (loadBackground(settings.screen_path, bg_img) != RESULTS::NO_ERROR)
		loadWidgets(settings.screen_path, widgets);
	return RESULTS::NO_ERROR;
}

RESULTS SaveManager::saveConfig()
{
	file = qspiFS.open(CONFIG_PATH, Adafruit_LittleFS_Namespace::FILE_O_WRITE);

	if (!file) return RESULTS::FILE_OPEN_ERROR;
	size_t writen = file.write(reinterpret_cast<uint8_t*>(&settings), sizeof(settings));
	file.close();

	if (writen != sizeof(settings)) return RESULTS::FILE_WRITE_ERROR;
	return RESULTS::NO_ERROR;
}

void SaveManager::loadDefaultScreen(std::vector<std::shared_ptr<Widget>>* widgets, BackgroundImage* bg_img)
{
	widgetManager.eraseWidgets(); // erasing all widgets

	bg_img->set_bitmap(my_2bpp_bitmap, 0, 0, 400, 300, 2);
	widgetManager.addWidget(std::make_shared<TextBox>("Miocasa's Eink", 0, 0, 400, 300,GxEPD_BLACK, GxEPD_WHITE, 2,
	                                                  Fonts::Monocraft9pt7b_ID, VAligns::Bottom, HAligns::Center));
}

RESULTS SaveManager::saveScreen(const char* path, std::vector<std::shared_ptr<Widget>>* widgets,
                                BackgroundImage* bg_img)
{
	RESULTS res = saveBackground(path, bg_img);
	if (res != RESULTS::NO_ERROR) return RESULTS::SAVE_BACKGROUND_ERROR;

	res = saveWidgets(path, widgets);
	if (res != RESULTS::NO_ERROR) return RESULTS::SAVE_WIDGETS_ERROR;
	return res;
}


RESULTS SaveManager::saveWidgets(const char* path, std::vector<std::shared_ptr<Widget>>* widgets)
{
	if (!path)
	{
		_path_buffer[0] = '\0';
		_size = 0;
		return RESULTS::INVALID_POINTER_ERROR;
	}

	qspiFS.mkdir(path);
	for (uint16_t i = 0; i < widgets->size(); i++)
	{
		create_path(path, i);
		auto widget = widgets->begin() + i;

		RESULTS st = saveWidget(_path_buffer, &*widget);
		if (st != RESULTS::NO_ERROR) return st;
	}

	return RESULTS::NO_ERROR;
}

RESULTS SaveManager::loadWidgets(const char* path, std::vector<std::shared_ptr<Widget>>* widgets)
{
	if (!path)
	{
		_path_buffer[0] = '\0';
		_size = 0;
		return RESULTS::INVALID_POINTER_ERROR;
	}

	Adafruit_LittleFS_Namespace::File dir = qspiFS.open(path);
	if (!(dir.isDirectory() && dir)) return RESULTS::FILE_NOT_FOUND_ERROR;

	widgets->clear();

	while (Adafruit_LittleFS_Namespace::File entry = dir.openNextFile())
	{
		const char* file_path = entry.name();
		if (strstr(file_path, WIDGET_FILE_NAME_HEADER) == nullptr) continue;

		sprintf(_path_buffer, "%s/%s", path, entry.name()); //entry.name()
		auto res = loadWidget(_path_buffer, widgets);
		if (res != RESULTS::NO_ERROR) return res;
	}

	return RESULTS::NO_ERROR;
}

RESULTS SaveManager::saveBackground(const char* path, BackgroundImage* bg_img)
{
	if (!path)
	{
		_path_buffer[0] = '\0';
		_size = 0;
		return RESULTS::INVALID_POINTER_ERROR;
	}
	sprintf(_path_buffer, BACKGROUND_FILE_FORMAT, path);

	if (qspiFS.exists(_path_buffer)) qspiFS.remove(_path_buffer);
	if (!file.open(_path_buffer, Adafruit_LittleFS_Namespace::FILE_O_WRITE)) return RESULTS::FILE_OPEN_ERROR;

	// Write header
	widget_t header{
		// header general data for all widgets
		SAVEMANAGER_VERSION,
		bg_img->get_type(),
		bg_img->get_x(),
		bg_img->get_y(),
		bg_img->get_width(),
		bg_img->get_height(),
		bg_img->get_color(),
		bg_img->get_bg_color(),
	};

	size_t written = file.write(reinterpret_cast<const uint8_t*>(&header), sizeof(header));
	if (written != sizeof(header))
	{
		file.close();
		return RESULTS::FILE_WRITE_ERROR;
	} // Check for header writen,

	// Write data
	bg_image_t data{
		bg_img->get_bpp(),
		bg_img->get_size(),
	};

	written += file.write(reinterpret_cast<const uint8_t*>(&data), sizeof(data));
	written += file.write(bg_img->get_bitmap(), bg_img->get_size());
	file.close();

	// Check for widget body writen
	size_t expected = sizeof(header) + sizeof(data) + bg_img->get_size();
	return (written == expected) ? RESULTS::NO_ERROR : RESULTS::FILE_WRITE_ERROR;
}

RESULTS SaveManager::loadBackground(const char* path, BackgroundImage* bg_img)
{
	if (!path)
	{
		_path_buffer[0] = '\0';
		_size = 0;
		return RESULTS::INVALID_POINTER_ERROR;
	}
	sprintf(_path_buffer, BACKGROUND_FILE_FORMAT, path);

	if (!file.open(_path_buffer, Adafruit_LittleFS_Namespace::FILE_O_READ)) return RESULTS::FILE_OPEN_ERROR;
	widget_t header{};

	size_t _read = file.read(&header, sizeof(header));
	if (_read != sizeof(header)) return RESULTS::FILE_READ_ERROR;
	if (header._type != WidgetTypes::BG_IMAGE) return RESULTS::WIDGET_TYPE_ERROR;

	bg_image_t data{};

	_read = file.read(&data, sizeof(data));

	_read += file.read((void*)bg_img->get_bitmap(), data.size);

	if (_read != data.size + sizeof(data)) return RESULTS::FILE_READ_ERROR;

	bg_img->set_x(header._x);
	bg_img->set_y(header._y);
	bg_img->set_width(header._w);
	bg_img->set_height(header._h);
	bg_img->set_color(header._color);
	bg_img->set_bg_color(header._bg_color);

	bg_img->set_size(data.size);
	bg_img->set_bpp(data.bpp);

	return RESULTS::NO_ERROR;
}

void SaveManager::create_path(const char* path, uint8_t num)
{
	sprintf(_path_buffer, WIDGET_FILE_FORMAT, path, WIDGET_FILE_NAME_HEADER, num);
}

RESULTS SaveManager::saveWidget(const char* path, std::shared_ptr<Widget>* widget)
{
	switch (widget->get()->get_type())
	{
	default:
	case WidgetTypes::NONE:
		return RESULTS::WIDGET_TYPE_ERROR;
	case WidgetTypes::TEXT_BOX:
		saveTextBox(path, widget);
		break;
	}
	return RESULTS::NO_ERROR;
}

RESULTS SaveManager::loadWidget(const char* path, std::vector<std::shared_ptr<Widget>>* widgets)
{
	if (!file.open(path, Adafruit_LittleFS_Namespace::FILE_O_READ)) return RESULTS::FILE_OPEN_ERROR;
	widget_t header{};

	size_t _read = file.read(&header, sizeof(header));
	if (_read != sizeof(header)) return RESULTS::FILE_READ_ERROR;

	RESULTS st = RESULTS::ERROR_PLACEHOLDER;

	switch (header._type)
	{
	default:
	case WidgetTypes::NONE:
		file.close();
		return RESULTS::WIDGET_TYPE_ERROR;

	case WidgetTypes::TEXT_BOX:
		st = loadTextBox(path, widgets, &header);
		break;
	}
	file.close();
	return st;
}

RESULTS SaveManager::saveTextBox(const char* path, std::shared_ptr<Widget>* widget)
{
	auto casted = static_cast<TextBox*>(widget->get());
	if (!casted) return RESULTS::WIDGET_CAST_ERROR;

	if (qspiFS.exists(path)) qspiFS.remove(path);
	if (!file.open(path, Adafruit_LittleFS_Namespace::FILE_O_WRITE)) return RESULTS::FILE_OPEN_ERROR;

	// Write header
	widget_t header{
		// header general data for all widgets
		SAVEMANAGER_VERSION,
		casted->get_type(),
		casted->get_x(),
		casted->get_y(),
		casted->get_width(),
		casted->get_height(),
		casted->get_color(),
		casted->get_bg_color(),
	};

	size_t written = file.write(reinterpret_cast<const uint8_t*>(&header), sizeof(header));
	if (written != sizeof(header))
	{
		file.close();
		return RESULTS::FILE_WRITE_ERROR;
	} // Check for header writen,

	// Write data
	text_box_t data{
		casted->font_size(),
		casted->font(),
		casted->valign(),
		casted->halign(),
		casted->size(),
	};

	written += file.write(reinterpret_cast<const uint8_t*>(&data), sizeof(data));
	written += file.write(casted->str(), casted->size());
	file.close();

	// Check for widget body writen
	size_t expected = sizeof(header) + sizeof(data) + casted->size();
	return (written == expected) ? RESULTS::NO_ERROR : RESULTS::FILE_WRITE_ERROR;
}

RESULTS SaveManager::loadTextBox(const char* path, std::vector<std::shared_ptr<Widget>>* widgets, widget_t* header)
{
	text_box_t data{};


	size_t _read = file.read(&data._font_size, sizeof(data));

	char _str[TEXT_BUFFER_SIZE];
	_str[min(data._size, TEXT_BUFFER_SIZE - 1)] = '\0';

	_read += file.read(&_str, data._size);

	if (_read != sizeof(data) + data._size) return RESULTS::FILE_READ_ERROR;

	widgets->push_back(std::make_shared<TextBox>(_str, header, &data));

	return RESULTS::NO_ERROR;
}
