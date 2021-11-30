#ifndef NALL_QT_CHECKDELEGATE_HPP
#define NALL_QT_CHECKDELEGATE_HPP

namespace nall {

class CheckDelegate : public QItemDelegate {
  Q_OBJECT

public:
  CheckDelegate(QObject *parent = 0);
  
  QWidget* createEditor(QWidget*, const QStyleOptionViewItem&, const QModelIndex&) const;
  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

private:
  QRect CheckBoxRect(const QStyleOptionViewItem &view_item_style_options) const;

  QPixmap pixmapOn, pixmapOff;
  QRect check_box_rect;
};

inline CheckDelegate::CheckDelegate(QObject *parent)
  : QItemDelegate(parent) {
  
  // Rendering the checkboxes with native OS style is expensive, so just generate
  // two fixed images representing the two main states of the checkbox
  // and render with those instead
  QCheckBox checkbox;
  QStyleOptionButton check_box_style_option;
  check_box_style_option.initFrom(&checkbox);
  check_box_rect = checkbox.style()->subElementRect(
                   QStyle::SE_CheckBoxIndicator, &check_box_style_option, &checkbox);

  pixmapOff = QPixmap(this->check_box_rect.size());
  pixmapOff.fill(QColor(0, 0, 0, 0));
  checkbox.setChecked(false);
  checkbox.render(&this->pixmapOff, QPoint(), QRegion(), QWidget::DrawChildren);

  pixmapOn = QPixmap(this->check_box_rect.size());
  pixmapOn.fill(QColor(0, 0, 0, 0));
  checkbox.setChecked(true);
  checkbox.render(&this->pixmapOn, QPoint(), QRegion(), QWidget::DrawChildren);
}

inline QRect CheckDelegate::CheckBoxRect(const QStyleOptionViewItem &option) const {
  QPoint check_box_point(option.rect.x() + option.rect.width() / 2 - this->check_box_rect.width() / 2,
                         option.rect.y() + option.rect.height() / 2 - this->check_box_rect.height() / 2);
  return QRect(check_box_point, this->check_box_rect.size());
}

inline QWidget* CheckDelegate::createEditor(QWidget *, const QStyleOptionViewItem &, const QModelIndex &) const {
  return new QCheckBox();
}

inline void CheckDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
  //paint the background
  const QWidget *widget = option.widget;
  QStyle *style = widget ? widget->style() : QApplication::style();
  style->drawControl(QStyle::CE_ItemViewItem, &option, painter, widget);

  QRect rect = CheckBoxRect(option);

  if (index.model()->data(index, Qt::EditRole).toBool()) {
      drawCheck(painter, option, rect, Qt::Checked);
      drawFocus(painter, option, rect);
  } else {
      drawCheck(painter, option, rect, Qt::Unchecked);
      drawFocus(painter, option, rect);
  }
}

inline bool CheckDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {
  if ((event->type() == QEvent::MouseButtonRelease) || (event->type() == QEvent::MouseButtonDblClick)) {
	
	QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);
	if (mouse_event->button() != Qt::LeftButton || !CheckBoxRect(option).contains(mouse_event->pos())) 
	  return false;
	if (event->type() == QEvent::MouseButtonDblClick) 
	  return true;

  } else if (event->type() == QEvent::KeyPress) {
	
	QKeyEvent *key_event = static_cast<QKeyEvent*>(event);
	if (key_event->key() != Qt::Key_Space && key_event->key() != Qt::Key_Select) 
	  return false;
	  
  } else return false;

  bool checked = index.model()->data(index, Qt::EditRole).toBool();
  model->setData(index, !checked);
  return true;
}

}

#endif
