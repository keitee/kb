#include <QCoreApplication>

class StateMachine : public QObject
{
Q_OBJECT

enum State {
    RunningSuperState,
        StartingDiscoveryState,
        DiscoveringState,
        StoppingDiscoveryState,
    FinishedState
};
Q_ENUM(State)

};

int main(int argc, char *argv[])
{
    quint32 value1;

    uint value2;

    QCoreApplication a(argc, argv);

    return a.exec();
}
