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
    QCoreApplication a(argc, argv);

    return a.exec();
}
