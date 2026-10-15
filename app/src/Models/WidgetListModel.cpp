#include "WidgetListModel.h"
#include "Protocol.h"

WidgetListModel::WidgetListModel(QObject* parent) : QAbstractListModel(parent) {}

int WidgetListModel::rowCount(const QModelIndex& parent) const {
  return parent.isValid() ? 0 : count();
}

QString WidgetListModel::label(int index, const TextBoxSpec& spec) {
  QString text = spec.text.simplified();
  if (text.size() > 28) {
    text = text.left(28) + "…";
  }
  if (text.isEmpty()) {
    text = "(empty)";
  }
  return QString("%1: %2").arg(index).arg(text);
}

QVariant WidgetListModel::data(const QModelIndex& idx, int role) const {
  if (!idx.isValid() || idx.row() < 0 || idx.row() >= count()) {
    return {};
  }
  const TextBoxSpec& s = items_[static_cast<size_t>(idx.row())];
  switch (role) {
    case Qt::DisplayRole:
      return label(idx.row(), s);
    case TextRole:
      return s.text;
    case XRole:
      return s.x;
    case YRole:
      return s.y;
    case WRole:
      return s.w;
    case HRole:
      return s.h;
    case FontSizeRole:
      return s.fontSize;
    case FontIdRole:
      return s.fontId;
    case VAlignRole:
      return s.valign;
    case HAlignRole:
      return s.halign;
    case ColorRole:
      return s.color;
    case BgColorRole:
      return s.bgColor;
    default:
      return {};
  }
}

QHash<int, QByteArray> WidgetListModel::roleNames() const {
  auto roles = QAbstractListModel::roleNames();
  roles[TextRole] = "text";
  roles[XRole] = "x";
  roles[YRole] = "y";
  roles[WRole] = "w";
  roles[HRole] = "h";
  roles[FontSizeRole] = "fontSize";
  roles[FontIdRole] = "fontId";
  roles[VAlignRole] = "valign";
  roles[HAlignRole] = "halign";
  roles[ColorRole] = "color";
  roles[BgColorRole] = "bgColor";
  return roles;
}

int WidgetListModel::add(int copyFrom) {
  TextBoxSpec spec;
  if (copyFrom >= 0 && copyFrom < count()) {
    spec = items_[static_cast<size_t>(copyFrom)];
  } else {
    spec.text = "Text";
  }

  const int row = count();
  beginInsertRows({}, row, row);
  items_.push_back(spec);
  endInsertRows();

  emit countChanged();
  emit specsChanged();
  return row;
}

void WidgetListModel::remove(int row) {
  if (row < 0 || row >= count()) {
    return;
  }
  beginRemoveRows({}, row, row);
  items_.erase(items_.begin() + row);
  endRemoveRows();

  relabelAll();
  emit countChanged();
  emit specsChanged();
}

void WidgetListModel::clear() {
  if (items_.empty()) {
    return;
  }
  beginResetModel();
  items_.clear();
  endResetModel();

  emit countChanged();
  emit specsChanged();
}

void WidgetListModel::relabelAll() {
  if (!items_.empty()) {
    emit dataChanged(index(0), index(count() - 1), {Qt::DisplayRole});
  }
}

QVariantMap WidgetListModel::get(int row) const {
  if (row < 0 || row >= count()) {
    return {};
  }
  const TextBoxSpec& s = items_[static_cast<size_t>(row)];
  return {
      {"text", s.text},         {"x", s.x},           {"y", s.y},           {"w", s.w},           {"h", s.h},
      {"fontSize", s.fontSize}, {"fontId", s.fontId}, {"valign", s.valign}, {"halign", s.halign}, {"color", s.color},
      {"bgColor", s.bgColor}};
}

bool WidgetListModel::setField(int row, const QString& name, const QVariant& v) {
  if (row < 0 || row >= count()) {
    return false;
  }
  TextBoxSpec& s = items_[static_cast<size_t>(row)];

  if (name == "text") {
    const QString t = v.toString();
    if (t.toUtf8().size() > Layout::kMaxTextBytes) {
      return false;
    }
    s.text = t;
  } else if (name == "x") {
    s.x = v.toInt();
  } else if (name == "y") {
    s.y = v.toInt();
  } else if (name == "w") {
    s.w = v.toInt();
  } else if (name == "h") {
    s.h = v.toInt();
  } else if (name == "fontSize") {
    s.fontSize = v.toInt();
  } else if (name == "fontId") {
    s.fontId = v.toInt();
  } else if (name == "valign") {
    s.valign = v.toInt();
  } else if (name == "halign") {
    s.halign = v.toInt();
  } else if (name == "color") {
    s.color = v.toInt();
  } else if (name == "bgColor") {
    s.bgColor = v.toInt();
  } else {
    return false;
  }

  emit dataChanged(index(row), index(row));
  emit specsChanged();
  return true;
}
