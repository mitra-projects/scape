#include "ImageFileDialog.h"

#include <OgreString.h>
#include <OgreStringConverter.h>

ImageFileDialog::ImageFileDialog(QWidget* parent) : QDialog(parent)
{
    mImageFileDialogUI = new Ui::ImageFileDialog();
    mImageFileDialogUI->setupUi(this);

    connectActions();
}

ImageFileDialog::~ImageFileDialog() { delete mImageFileDialogUI; }

std::string ImageFileDialog::getSelectedFormatName() const
{
    return mFormatItemVector[mImageFileDialogUI->formatComboBox->currentIndex()].name;
}

bool ImageFileDialog::getFlipX() const { return mImageFileDialogUI->flipxCheckBox->isChecked(); }

bool ImageFileDialog::getFlipY() const { return mImageFileDialogUI->flipyCheckBox->isChecked(); }

std::string ImageFileDialog::getFilePath() const
{
    return mImageFileDialogUI->fileLineEdit->text().toStdString();
}

int ImageFileDialog::getWidth() const
{
    if (mImageFileDialogUI->widthLineEdit->isEnabled())
    {
        const Ogre::String widthText =
            Ogre::String(mImageFileDialogUI->widthLineEdit->text().toStdString());
        return Ogre::StringConverter::parseInt(widthText, -1);
    }
    return -1;
}

int ImageFileDialog::getHeight() const
{
    if (mImageFileDialogUI->heightLineEdit->isEnabled())
    {
        const Ogre::String heightText =
            Ogre::String(mImageFileDialogUI->heightLineEdit->text().toStdString());
        return Ogre::StringConverter::parseInt(heightText, -1);
    }
    return -1;
}

int ImageFileDialog::getEndiannessIndex() const
{
    if (mImageFileDialogUI->endiannessComboBox->isEnabled())
    {
        return mImageFileDialogUI->endiannessComboBox->currentIndex();
    }
    return -1;
}

int ImageFileDialog::getBppIndex() const
{
    if (mImageFileDialogUI->bppComboBox->isEnabled())
    {
        return mImageFileDialogUI->bppComboBox->currentIndex();
    }
    return -1;
}

void ImageFileDialog::populate(const FormatItemVector& formatItemVector)
{
    mImageFileDialogUI->formatComboBox->clear();

    mFormatItemVector = formatItemVector;

    for (FormatItemVector::const_iterator it = mFormatItemVector.begin();
         it != mFormatItemVector.end(); it++)
    {
        mImageFileDialogUI->formatComboBox->addItem(QString(it->label.c_str()));
    }
}

int ImageFileDialog::getFormatIndexFromExtension(const std::string& extension)
{
    for (FormatItemVector::const_iterator it = mFormatItemVector.begin(); it != mFormatItemVector.end();
         it++)
    {
        Ogre::String fileFilterString = it->formatExtensions;
        std::vector<Ogre::String> fileFilters = Ogre::StringUtil::split(fileFilterString, ";");
        for (std::vector<Ogre::String>::const_iterator fileFilterIt = fileFilters.begin();
             fileFilterIt != fileFilters.end(); fileFilterIt++)
        {
            if (Ogre::StringUtil::match(extension, *fileFilterIt, false))
            {
                return it - mFormatItemVector.begin();
            }
        }
    }
    return -1;
}

void ImageFileDialog::selectedFormatChanged(int index)
{
    mImageFileDialogUI->formatDescriptionTextEdit->setPlainText(
        QString(mFormatItemVector[index].description.c_str()));
}

void ImageFileDialog::selectFileButtonClicked() {}

void ImageFileDialog::fileLineEditTextChanged(const QString& text) {}

void ImageFileDialog::connectActions()
{
    connect(mImageFileDialogUI->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(mImageFileDialogUI->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(mImageFileDialogUI->formatComboBox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(selectedFormatChanged(int)));
    connect(mImageFileDialogUI->openFilePushButton, SIGNAL(clicked()), this,
            SLOT(selectFileButtonClicked()));
    connect(mImageFileDialogUI->fileLineEdit, SIGNAL(textChanged(const QString&)), this,
            SLOT(fileLineEditTextChanged(const QString&)));
}
