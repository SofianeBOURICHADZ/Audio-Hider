#include "MainWindow.h"
#include "Engine.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtCore/QFileInfo>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Audio Encryption & Steganography Tool");
    resize(550, 480);

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // File info header
    QHBoxLayout *fileRow = new QHBoxLayout();
    m_fileLabel = new QLabel("Loaded File: None", this);
    m_fileLabel->setStyleSheet("font-weight: bold; font-size: 13px;");
    m_btnChangeFile = new QPushButton("Load Different File", this);
    fileRow->addWidget(m_fileLabel);
    fileRow->addWidget(m_btnChangeFile);
    mainLayout->addLayout(fileRow);

    // Credentials Input Fields
    QFormLayout *formLayout = new QFormLayout();
    m_pwdInput = new QLineEdit(this);
    m_pwdInput->setPlaceholderText("Set or enter your file password...");
    m_pwdInput->setEchoMode(QLineEdit::Password);

    m_keyInput = new QLineEdit(this);
    m_keyInput->setPlaceholderText("Generated automatically on encrypt / Enter to decrypt...");

    m_codeInput = new QLineEdit(this);
    m_codeInput->setPlaceholderText("Generated numerical code / Enter to decrypt...");

    formLayout->addRow("1. Password (User):", m_pwdInput);
    formLayout->addRow("2. Key (Program Generated):", m_keyInput);
    formLayout->addRow("3. Code (Numerical):", m_codeInput);
    mainLayout->addLayout(formLayout);

    // Action Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    m_btnEncrypt = new QPushButton("Encrypt & Hide Message", this);
    m_btnDecrypt = new QPushButton("Decrypt Message", this);
    btnLayout->addWidget(m_btnEncrypt);
    btnLayout->addWidget(m_btnDecrypt);
    mainLayout->addLayout(btnLayout);

    // Message Display
    m_textArea = new QTextEdit(this);
    m_textArea->setPlaceholderText("Type secret message to hide OR extracted result will display here...");
    mainLayout->addWidget(m_textArea);

    setCentralWidget(centralWidget);

    connect(m_btnChangeFile, &QPushButton::clicked, this, &MainWindow::changeFile);
    connect(m_btnEncrypt, &QPushButton::clicked, this, &MainWindow::handleEncrypt);
    connect(m_btnDecrypt, &QPushButton::clicked, this, &MainWindow::handleDecrypt);
}

bool MainWindow::selectInitialFile() {
    QString filePath = QFileDialog::getOpenFileName(this, "Select Cover Audio File", "", "WAV Audio Files (*.wav)");
    if (filePath.isEmpty()) {
        return false;
    }
    m_currentFilePath = filePath;
    m_fileLabel->setText("Loaded File: " + QFileInfo(filePath).fileName());
    return true;
}

void MainWindow::changeFile() {
    selectInitialFile();
}

void MainWindow::handleEncrypt() {
    if (m_currentFilePath.isEmpty()) {
        QMessageBox::warning(this, "Error", "No audio file loaded!");
        return;
    }

    QString userPassword = m_pwdInput->text();
    if (userPassword.isEmpty()) {
        QMessageBox::warning(this, "Missing Input", "Please enter a password first!");
        return;
    }

    QString message = m_textArea->toPlainText();
    if (message.isEmpty()) {
        QMessageBox::warning(this, "Missing Input", "Please type a secret message to hide!");
        return;
    }

    // Generate random key and numerical code
    EncryptionCredentials creds = Engine::generateCredentials(userPassword.toStdString());

    // Populate GUI fields so user can view/copy them
    m_keyInput->setText(QString::fromStdString(creds.key));
    m_codeInput->setText(QString::fromStdString(creds.code));

    QString outPath = QFileDialog::getSaveFileName(this, "Save Encrypted Audio As", "", "WAV Audio Files (*.wav)");
    if (outPath.isEmpty()) return;

    bool ok = Engine::encodeMessage(m_currentFilePath.toStdString(),
                                   outPath.toStdString(),
                                   creds,
                                   message.toStdString());

    if (ok) {
        QMessageBox::information(this, "Success", 
            QString("Message encrypted!\n\nCredentials needed to decrypt:\nKey: %1\nNumerical Code: %2")
            .arg(QString::fromStdString(creds.key))
            .arg(QString::fromStdString(creds.code)));
    } else {
        QMessageBox::critical(this, "Error", "Audio file is too small to fit this message payload.");
    }
}

void MainWindow::handleDecrypt() {
    if (m_currentFilePath.isEmpty()) {
        QMessageBox::warning(this, "Error", "No audio file loaded!");
        return;
    }

    EncryptionCredentials creds;
    creds.password = m_pwdInput->text().toStdString();
    creds.key = m_keyInput->text().toStdString();
    creds.code = m_codeInput->text().toStdString();

    std::string result = Engine::decodeMessage(m_currentFilePath.toStdString(), creds);
    m_textArea->setPlainText(QString::fromStdString(result));
}