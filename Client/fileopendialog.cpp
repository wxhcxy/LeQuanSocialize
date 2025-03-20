
#include "fileopendialog.h"

#include "client.h"
#include "clientInfo.h"
#include <boost/json.hpp>

#include <QBuffer>
#include <QApplication>
#include <QDebug>
#include <QQuickWindow>

FileOpenDialog::FileOpenDialog(QQuickItem *parent)
    : QQuickItem(parent)
    , m_dlgHelper(init_helper())
    , m_modality(Qt::WindowModal)
    #if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    , m_options(QFileDialogOptions::create())
    #else
    , m_options(QSharedPointer<QFileDialogOptions>(new QFileDialogOptions()))
    #endif
{
    QApplication *appHasQtWidgetsSupport = qobject_cast<QApplication *>(QCoreApplication::instance());
    Q_ASSERT(appHasQtWidgetsSupport);
    Q_UNUSED(appHasQtWidgetsSupport);

    if (valid())
    {
        connect(m_dlgHelper, &QPlatformFileDialogHelper::accept,
                this, &FileOpenDialog::accept);
        connect(m_dlgHelper, &QPlatformFileDialogHelper::reject,
                this, &FileOpenDialog::reject);
    }
}

FileOpenDialog::~FileOpenDialog()
{
    if (m_dlgHelper)
        m_dlgHelper->hide();
    delete m_dlgHelper;
}

bool FileOpenDialog::valid() const
{
    if (m_dlgHelper) return true;
    else return false;
}

QUrl FileOpenDialog::fileUrl() const
{
    return fileUrl_;
}

QList<QUrl> FileOpenDialog::fileUrls() const
{
    return fileUrls_;
}

void FileOpenDialog::setFileUrl(QUrl fileUrl)
{
    if (fileUrl_ != fileUrl)
    {
        fileUrl_ = fileUrl;
        emit fileUrlChanged();
    }
}

void FileOpenDialog::setFileUrls(QList<QUrl> fileUrls)
{
    if (fileUrls_ != fileUrls)
    {
        fileUrls_ = fileUrls;
        emit fileUrlsChanged();
    }
}

void FileOpenDialog::publishJotting(QString content)
{
    qDebug() << "发布笔记";
    qDebug() << content;

    // 创建 "jottingData" 对象
    boost::json::object jottingData;
    jottingData["content"] = content.toStdString();

    // 初始化 "materials" 为一个数组
    jottingData["materials"] = boost::json::array();

    QVector<QPixmap> pics;
    for (const auto &path : fileUrls_) {
        QPixmap pic;
        std::string path1 = path.toEncoded().toStdString();
        pic.load(QString::fromStdString(path1));
        pics.push_back(pic);

        qDebug() << "path:" << QString::fromStdString(path1);
        if (pic.isNull()) {
            qDebug() << "图片是空的";
        }

        // 将图片进行编码后传输
        QByteArray imageData;
        QBuffer buffer(&imageData);
        if (pic.save(&buffer, "JPG")) {
            imageData = imageData.toBase64();
            QString str = QString::fromUtf8(imageData);
            jottingData["materials"].as_array().emplace_back(str.toStdString());

            qDebug() << str;
        } else {
            qDebug() << "图片保存失败";
        }
    }

    // 创建 "message" 对象
    boost::json::object message;
    message["id"] = ID;
    message["request"] = "PublishJotting";
    message["jotting"] = jottingData;

    // 序列化 JSON 对象为字符串
    std::string mes = boost::json::serialize(message);
    std::cout << "开始发送笔记信息:" << std::endl << mes << std::endl;

    // 发送数据
    Client::getInstance()->reconnect();
    Client::getInstance()->send(mes.c_str(), mes.size());

    std::cout << "发布笔记完成！" << std::endl;
}

QString FileOpenDialog::filename() const
{
    return filename_;
}

void FileOpenDialog::setFilename(QString filename)
{
    if (filename_ != filename)
    {
        filename_ = filename;
        emit filenameChanged();
    }
}

QString FileOpenDialog::title() const
{
    return title_;
}

void FileOpenDialog::setTitle(QString title)
{
    if (title_ != title)
    {
        title_ = title;
        emit titleChanged();
    }
}

QStringList FileOpenDialog::nameFilters() const
{
    return nameFilters_;
}

void FileOpenDialog::setNameFilters(QStringList nameFilters)
{
    if (nameFilters_ != nameFilters)
    {
        nameFilters_ = nameFilters;
        emit nameFiltersChanged();
    }
}

bool FileOpenDialog::selectMultiple() const
{
    return selectMultiple_;
}

void FileOpenDialog::setSelectMultiple(bool selectMultiple)
{
    if (selectMultiple_ != selectMultiple)
    {
        selectMultiple_ = selectMultiple;
        emit selectMultipleChanged();
    }
}

QPlatformFileDialogHelper* FileOpenDialog::init_helper()
{
    return static_cast<QPlatformFileDialogHelper*>(
                QGuiApplicationPrivate::platformTheme()->createPlatformDialogHelper(QPlatformTheme::FileDialog)
                );
}

void FileOpenDialog::open()
{
    if (!valid()) return;

    QQuickItem *parent = this->parentItem();
    Q_ASSERT(parent);

    QQuickWindow *window = parent->window();
    Q_ASSERT(window);

    m_parentWindow = window;

    m_options->setFileMode(selectMultiple_ ? QFileDialogOptions::ExistingFiles : QFileDialogOptions::ExistingFile);
    m_options->setAcceptMode(QFileDialogOptions::AcceptOpen);
    m_options->setWindowTitle(title());
    m_options->setNameFilters(nameFilters());

#ifdef Q_OS_OSX
    QString initialSelection = QFileInfo(QDir::homePath(), filename()).absoluteFilePath();
    qDebug() << "Initial file:" << initialSelection;
    m_options->setInitiallySelectedFiles(QList<QUrl>() << QUrl::fromLocalFile(initialSelection));
#endif
#ifdef Q_OS_WIN
    qDebug() << "Initial filename:" << filename();
    m_options->setInitiallySelectedFiles(QList<QUrl>() << QUrl::fromLocalFile(filename()));
#endif
#ifdef Q_OS_LINUX
    qDebug() << "Initial directory:" << QDir::homePath();
    m_dlgHelper->setDirectory(QUrl::fromLocalFile(QDir::homePath()));
#endif

    m_dlgHelper->setOptions(m_options);
    m_dlgHelper->setFilter(); // applyOptions();

    Qt::WindowFlags flags = Qt::Dialog;
    if (!title().isEmpty()) flags |= Qt::WindowTitleHint;

    m_visible = m_dlgHelper->show(flags, m_modality, m_parentWindow);
}

void FileOpenDialog::close()
{
    if (!valid()) return;

    m_dlgHelper->hide();
    m_visible = false;
}

void FileOpenDialog::accept()
{
    if (!valid()) return;

    m_dlgHelper->hide();

    QList<QUrl> selectedUrls = m_dlgHelper->selectedFiles();
    if (!selectedUrls.empty())
    {
        if (selectedUrls.size() == 1)
            setFileUrl(selectedUrls.at(0));
        else
            setFileUrl();

        setFileUrls(selectedUrls);
    }

    emit accepted();
}

void FileOpenDialog::reject()
{
    if (!valid()) return;

    m_dlgHelper->hide();
    emit rejected();
}
