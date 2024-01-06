#ifndef CAPTUREPREVIEW_H
#define CAPTUREPREVIEW_H

#include <QList>
#include <QWidget>
#include "filtering.h"
#include "kinect_types.h"
#include "qdatetime.h"
namespace Ui {
class CapturePreview;
}

class CapturePreview : public QWidget {
    Q_OBJECT

public:
    friend QDataStream &operator<<(QDataStream &out, const CapturePreview &item);

    explicit CapturePreview(QWidget* parent = nullptr);
    CapturePreview(int id,
                   VideoCapture cap,
                   PointFilter::Filter pc,
                   QDateTime time,
                   QWidget *parent = nullptr);
    ~CapturePreview();
    bool is_shown() const;
    PointCloud::Ptr points();
    int get_id() const;

    QDateTime get_time() const;

    void points_to_file(const std::string &fname);

public slots:
    void set_shown(bool s);
    void checkbox_changed();
signals:
    void visibility_changed();

private:
    Ui::CapturePreview* ui;
    std::vector<uint8_t> thumbnail;
    PointCloud::Ptr pts;
    QDateTime time;
    int id;
};

// QDataStream &operator<<(QDataStream &out, const CapturePreview &item);

#endif // CAPTUREPREVIEW_H
