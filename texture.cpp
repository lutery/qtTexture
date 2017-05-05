#include "texture.h"
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtQuick/QQuickWindow>
#include <QtGui/QOpenGLContext>

class TextureRenderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    TextureRenderer(){}
    ~TextureRenderer(){}

    void setViewportSize(const QSize& size)
    {
        mViewPortSize = size;
    }

    void setWindow(QQuickWindow* window)
    {
        mpWindow = window;
    }

public slots:
    void paint();

private:
    QSize mViewPortSize;
    qreal mT;
    QOpenGLShaderProgram* mpProgram;
    QQuickWindow* mpWindow;
};

void TextureRenderer::paint()
{
    if (!mpProgram)
    {
        initializeOpenGLFunctions();

        mpProgram = new QOpenGLShaderProgram();
        mpProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                           "attribute highp vec4 vertices;"
                                           "varying highp vec2 coords;"
                                           "void main() {"
                                           "    gl_Position = vertices;"
                                           "    coords = vertices.xy;"
                                           "}");

        mpProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                           "uniform lowp float t;"
                                           "varying highp vec2 coords;"
                                           "void main() {"
                                           "    lowp float i = 1. - (pow(abs(coords.x), 4.) + pow(abs(coords.y), 4.));"
                                           "    i = smoothstep(t - 0.8, t + 0.8, i);"
                                           "    i = floor(i * 20.) / 20.;"
                                           "    gl_FragColor = vec4(coords * .5 + .5, i, i);"
                                           "}");

        mpProgram->bindAttributeLocation("vertices", 0);
        mpProgram->link();
    }

    mpProgram->bind();
    mpProgram->enableAttributeArray(0);

    float values[] =
    {
        -1, -1,
         1, -1,
        -1,  1,
         1,  1
    };

    mpProgram->setAttributeArray(0, GL_FLOAT, values, 2);

    glViewport(0, 0, mViewPortSize.width(), mViewPortSize.height());

    glDisable(GL_DEPTH_TEST);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);


}

Texture::Texture()
{
    mRenderer = nullptr;
    connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(handleWindowChanged(QQuickWindow*)));
}

void Texture::sync()
{
    if (!mpRenderer)
    {
        mpRenderer = new TextureRenderer();
        connect(window(), SIGNAL(beforeRendering()), mpRenderer, SLOT(paint()), Qt::DirectConnection);
    }

    mpRenderer->setViewportSize(window()->size() * window()->devicePixelRatio());
    mpRenderer->setWindow(window());
}

void Texture::cleanup()
{
    if (mpRenderer)
    {
        delete mpRenderer;
        mpRenderer = nullptr;
    }
}

void Texture::handleWindowChanged(QQuickWindow *win)
{
    if (win)
    {
        connect(win, SIGNAL(beforeSynchronizing()), this, SLOT(sync()), Qt::DirectConnection);
        connect(win, SIGNAL(sceneGraphInvalidated()), this, SLOT(cleanup()), Qt::DirectConnection);

        win->setClearBeforeRendering(false);
    }
}
