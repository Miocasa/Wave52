#include "SaveManager.h"
#include "Widgets/TextBox.h"

using namespace Adafruit_LittleFS_Namespace;

SaveManager::SaveManager()
	: _path_buffer(), _size(0), file(InternalFS)
{
	_path_buffer[0] = '\0';
}

bool SaveManager::begin(bool format_on_fail)
{
	if (InternalFS.begin() || !format_on_fail) return false;
	InternalFS.format();
	return InternalFS.begin();
}

RESULTS SaveManager::saveWidgets(const char* path, std::vector<std::shared_ptr<Widget>>* widgets)
{
	if (!path)
	{
		_path_buffer[0] = '\0';
		_size = 0;
		return RESULTS::INVALID_POINTER_ERROR;
	}

	InternalFS.mkdir(path);
	for (uint16_t i = 0; i < widgets->size(); i++)
	{
		create_path(path, i);
		auto widget = widgets->begin() + i;

		saveWidget(_path_buffer, &*widget);
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

	File dir = InternalFS.open(path);
	if (!(dir.isDirectory() && dir)) return RESULTS::FILE_NOT_FOUND_ERROR;

	widgets->clear();

	while (File entry = dir.openNextFile())
	{
		auto res = loadWidget(entry.name(), widgets);
		if (res != RESULTS::NO_ERROR) return res;
	}

	return RESULTS::NO_ERROR;
}

RESULTS SaveManager::saveBackground(const char* path, std::shared_ptr<BackgroundImage>* bg_img)
{
	if (!path)
	{
		_path_buffer[0] = '\0';
		_size = 0;
		return RESULTS::INVALID_POINTER_ERROR;
	}

	return RESULTS::ERROR_PLACEHOLDER;
}

RESULTS SaveManager::loadBackground(const char* path, std::shared_ptr<BackgroundImage>* bg_img)
{
	if (!path)
	{
		_path_buffer[0] = '\0';
		_size = 0;
		return RESULTS::INVALID_POINTER_ERROR;
	}
	return RESULTS::ERROR_PLACEHOLDER;
}

void SaveManager::create_path(const char* path, uint8_t num)
{
	sprintf(_path_buffer, WIDGET_FILE_FORMAT, path, num);
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
	if (!file.open(path, FILE_O_READ)) return RESULTS::FILE_OPEN_ERROR;
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

	if (InternalFS.exists(path)) InternalFS.remove(path);
	if (!file.open(path, FILE_O_WRITE)) return RESULTS::FILE_OPEN_ERROR;

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
	if (written != sizeof(header)) return RESULTS::FILE_WRITE_ERROR; // Check for header writen,

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
