#pragma once

#include <QWidget>
#include "filtering.h"

namespace Ui {
class FilterWidget;
}

class FilterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FilterWidget(QWidget *parent = nullptr);
    ~FilterWidget();

    [[nodiscard]] virtual PointFilter::Filter get_filter() const = 0;

private:
    Ui::FilterWidget *ui;
};
