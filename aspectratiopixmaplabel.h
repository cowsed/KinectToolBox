#ifndef ASPECTRATIOPIXMAPLABEL_H
#define ASPECTRATIOPIXMAPLABEL_H

#include <QLabel>
#include <QPixmap>
#include <QResizeEvent>

/**
 * @brief The AspectRatioPixmapLabel class
 * A nice way to display an image is using a label and calling setPixmap on it
 * This class provides a class specialized for that functionality that will keep the aspect ratio of the image
 */
class AspectRatioPixmapLabel : public QLabel {
    Q_OBJECT
public:
    explicit AspectRatioPixmapLabel(QWidget* parent = 0);
    virtual int heightForWidth(int width) const;
    virtual QSize sizeHint() const;
    QPixmap scaledPixmap() const;
public slots:
    void setPixmap(const QPixmap&);
    void resizeEvent(QResizeEvent*);

private:
    QPixmap pix;
};

#endif // ASPECTRATIOPIXMAPLABEL_H
