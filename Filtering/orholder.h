#pragma once

#include <QWidget>
#include "Filtering/filterwidget.h"

namespace Ui {
class OrHolder;
}

class OrHolder : public FilterWidget
{
    Q_OBJECT

public:
    explicit OrHolder(QWidget *parent = nullptr);
    ~OrHolder();
    [[nodiscard]] PointFilter::Filter get_filter() const override;

private:
    Ui::OrHolder *ui;
};
