#pragma once

#include <QWidget>
#include "Filtering/filterwidget.h"

namespace Ui {
class AndHolder;
}

class AndHolder : public FilterWidget
{
    Q_OBJECT

public:
    explicit AndHolder(QWidget *parent = nullptr);
    ~AndHolder();

    PointFilter::Filter get_filter() const override;

private:
    Ui::AndHolder *ui;
};
