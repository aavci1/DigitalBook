#ifndef DEPTHANALYZER_H
#define DEPTHANALYZER_H

#include <QObject>

class DepthAnalyzerPrivate;

class DepthAnalyzer : public QObject {
  Q_OBJECT
public:
  enum Direction {
    NoDirection,
    Left,
    Right
  };

  DepthAnalyzer(QObject *parent = 0);
  ~DepthAnalyzer();

public slots:
  void analyze(const quint16 *buffer, const quint32 timestamp);

signals:
  void swipeRecognized(DepthAnalyzer::Direction direction);

private:
  DepthAnalyzerPrivate *d;
};

#endif // DEPTHANALYZER_H
