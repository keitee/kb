
// QLoggingCategory(const char *, QtMsgType severityLevel) (Qt 5.4)
//
// Both the QLoggingCategory constructor and the Q_LOGGING_CATEGORY() macro
// accept an optional QtMsgType argument, which disables all message types
// with a *lower severity*.
  
// define categories with default `level`
Q_LOGGING_CATEGOTY(milestone, "org.milestone", QtInfoMsg)
Q_LOGGING_CATEGOTY(prodlogs, "org.prodlogs", QtInfoMsg)
