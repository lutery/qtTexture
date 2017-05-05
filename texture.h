#ifndef TEXTURE_H
#define TEXTURE_H

//#include <QObject>
#include <QQuickItem>

class TextureRenderer;

class Texture : public QQuickItem
{
    Q_OBJECT
public:
    Texture();

signals:

public slots:
    void sync();
    void cleanup();

private slots:
    void handleWindowChanged(QQuickWindow* win);

private:
    TextureRenderer* mpRenderer;
};

#endif // TEXTURE_H
