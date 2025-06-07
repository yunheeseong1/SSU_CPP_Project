#include "inputworkhoursdialog.h"
#include "ui_inputworkhoursdialog.h"
#include <QMessageBox>

InputWorkHoursDialog::InputWorkHoursDialog(const QString &employeeName, int employeeIndex, const QDate &date, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputWorkHoursDialog),
    m_employeeIndex(employeeIndex),
    m_date(date),
    m_isEditMode(false),
    m_deleteRequested(false)
{
    ui->setupUi(this);
    setWindowTitle("근무 시간 입력");

    ui->employeeNameLabel->setText(QString("직원: %1").arg(employeeName));
    ui->dateLabel->setText(QString("날짜: %1").arg(m_date.toString("yyyy-MM-dd")));

    ui->startTimeEdit->setTime(QTime(18, 0)); // 기본 시작 시간
    ui->startTimeEdit->setFocus();
    ui->endTimeEdit->setTime(QTime(18, 0));   // 기본 종료 시간

    ui->deleteButton->hide(); // 입력 모드에서는 삭제 버튼 숨김
}

InputWorkHoursDialog::InputWorkHoursDialog(const QString &employeeName, int employeeIndex,
                                           const QDate &date, const WorkLog& existingLog, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputWorkHoursDialog),
    m_employeeIndex(employeeIndex),
    m_date(date),
    m_isEditMode(true),
    m_originalWorkLog(existingLog),
    m_deleteRequested(false)
{
    ui->setupUi(this);
    setWindowTitle("근무 시간 수정");

    ui->employeeNameLabel->setText(QString("직원: %1").arg(employeeName));
    ui->dateLabel->setText(QString("날짜: %1").arg(m_date.toString("yyyy-MM-dd")));

    ui->startTimeEdit->setTime(existingLog.getStartTime());
    ui->startTimeEdit->setFocus();
    ui->endTimeEdit->setTime(existingLog.getEndTime());

    // 삭제 버튼 활성화
    connect(ui->deleteButton, &QPushButton::clicked, this, &InputWorkHoursDialog::onDeleteButtonClicked);
}

InputWorkHoursDialog::~InputWorkHoursDialog()
{
    delete ui;
}

// 입력된 시간으로 WorkLog 생성
WorkLog InputWorkHoursDialog::getWorkLog() const
{
    QTime startTime = ui->startTimeEdit->time();
    QTime endTime = ui->endTimeEdit->time();
    return WorkLog(m_employeeIndex, m_date, startTime, endTime);
}

// 삭제 버튼 클릭 시 확인 후 삭제 요청 플래그 설정
void InputWorkHoursDialog::onDeleteButtonClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "근무 기록 삭제",
        "이 근무 기록을 삭제하시겠습니까?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        m_deleteRequested = true;
        accept();
    }
}
