#ifndef DEPTHANALYZER_H
#define DEPTHANALYZER_H

#include <QObject>

class DepthAnalyzerPrivate;

class DepthAnalyzer : public QObject {
    Q_OBJECT
public:
    DepthAnalyzer(QObject *parent = 0);
    ~DepthAnalyzer();

public slots:
    void updateData(uchar *image, ushort *depth, int width, int height);

signals:
    void swipeLeft();
    void swipeRight();

private:
    DepthAnalyzerPrivate *d;
};

#endif // DEPTHANALYZER_H
