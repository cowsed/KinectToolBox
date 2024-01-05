#include "componentbinaryexpression.h"
#include "ui_componentbinaryexpression.h"

ComponentBinaryExpression::ComponentBinaryExpression(QWidget *parent)
    : FilterWidget(parent)
    , ui(new Ui::ComponentBinaryExpression)
{
    ui->setupUi(this);
}

PointFilter::Filter ComponentBinaryExpression::get_filter() const
{

    float val = ui->doubleSpinBox->value();

    QString comp_s = ui->compComboBox->currentText();
    PointFilter::Component comp = PointFilter::Component::X;
    if (comp_s == "Y") {
        comp = PointFilter::Component::Y;
    } else if (comp_s == "Z") {
        comp = PointFilter::Component::Z;
    }
    QString rel = ui->relComboBox->currentText();

    if (rel == "<") {
        return PointFilter::ComponentLessThan(comp, val);
    } else {
        return PointFilter::ComponentGreaterThan(comp, val);
    }
}

ComponentBinaryExpression::~ComponentBinaryExpression()
{
    delete ui;
}
