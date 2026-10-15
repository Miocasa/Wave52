#pragma once

#include <QByteArray>
#include <QString>

struct TextBoxSpec
{
	QString text;
	int x = 10;
	int y = 10;
	int w = 200;
	int h = 40;
	int color = 0x0000;
	int bgColor = 0xFFFF;
	int fontSize = 1;
	int fontId = 0;
	int valign = 0;
	int halign = 0;
};

namespace WidgetSerializer {

	QByteArray textBox(const TextBoxSpec& spec);

}