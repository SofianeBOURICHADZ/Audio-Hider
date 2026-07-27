#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    bool selectInitialFile();

private slots:
    void handleEncrypt();
    void handleDecrypt();
    void changeFile();

private:
    QString m_currentFilePath;

    QLabel *m_fileLabel;
    QLineEdit *m_pwdInput;
    QLineEdit *m_keyInput;
    QLineEdit *m_codeInput;
    QTextEdit *m_textArea;

    QPushButton *m_btnEncrypt;
    QPushButton *m_btnDecrypt;
    QPushButton *m_btnChangeFile;
};

#endif // MAINWINDOW_H