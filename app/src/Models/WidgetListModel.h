#pragma once

#include <QAbstractListModel>
#include <QVariantMap>

#include <vector>

#include "WidgetSerializer.h"

class WidgetListModel : public QAbstractListModel {
  Q_OBJECT
  Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
  enum Role {
    TextRole = Qt::UserRole + 1,
    XRole,
    YRole,
    WRole,
    HRole,
    FontSizeRole,
    FontIdRole,
    VAlignRole,
    HAlignRole,
    ColorRole,
    BgColorRole
  };

  explicit WidgetListModel(QObject* parent = nullptr);

  int rowCount(const QModelIndex& parent = {}) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  int count() const {
    return static_cast<int>(items_.size());
  }
  const std::vector<TextBoxSpec>& specs() const {
    return items_;
  }
 Q_INVOKABLE int add(int copyFrom = -1);
  Q_INVOKABLE void remove(int row);
  Q_INVOKABLE void clear();
  Q_INVOKABLE QVariantMap get(int row) const;
  // name: text, x, y, w, h, fontSize, fontId, valign, halign, color, bgColor
  Q_INVOKABLE bool setField(int row, const QString& name, const QVariant& value);

signals:
  void countChanged();
  void specsChanged();

private:
  static QString label(int index, const TextBoxSpec& spec);
  void relabelAll();

  std::vector<TextBoxSpec> items_;
};
