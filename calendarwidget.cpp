#include "calendarwidget.h"
#include "ui_calendarwidget.h"
#include <QPushButton>
#include <QDate>
#include <QDebug>
#include <QLabel> // QLabel을 사용하므로 추가
#include "worklog.h"
#include <QVariant>


// 생성자: 달력 위젯의 초기 설정을 담당
CalendarWidget::CalendarWidget(DataManager *dataManager, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CalendarWidget),
    m_dataManager(dataManager)
{
    ui->setupUi(this); // .ui 파일 로드


    currentDate = QDate::currentDate(); // 현재 날짜로 초기화
    // 이전/다음 달 버튼과 날짜 버튼들에 대한 시그널-슬롯 연결
    connect(ui->btn_prev, &QPushButton::clicked, this, &CalendarWidget::showPreviousMonth);
    connect(ui->btn_next, &QPushButton::clicked, this, &CalendarWidget::showNextMonth);

    // 5x7개의 모든 날짜 버튼을 찾아서 클릭 이벤트(onDayButtonClicked)와 연결
    for (int row = 1; row <= 5; ++row) {
        for (int col = 0; col < 7; ++col) {
            QString buttonName = QString("btn_%1_%2").arg(row).arg(col);
            QPushButton *dayButton = findChild<QPushButton*>(buttonName);
            if (dayButton) {
                connect(dayButton, &QPushButton::clicked, this, &CalendarWidget::onDayButtonClicked);
            }
        }
    }
    updateCalendar(); // 위젯이 생성될 때 달력을 한 번 그림
}

CalendarWidget::~CalendarWidget()
{
    delete ui;
}


// 달력의 모든 셀을 현재 데이터에 맞게 다시 그리는 핵심 함수
void CalendarWidget::updateCalendar()
{
    qDebug() << "--- CalendarWidget::updateCalendar() CALLED for month:" << currentDate.toString("yyyy-MM") << "---";

    ui->label_month->setText(currentDate.toString("yyyy년 M월")); // "YYYY년 M월" 텍스트 설정

    QDate firstDayOfMonth(currentDate.year(), currentDate.month(), 1);
    int startDayOfWeek = firstDayOfMonth.dayOfWeek(); // 해당 월의 시작 요일 (월=1, 일=7)
    int daysInMonth = currentDate.daysInMonth();      // 해당 월의 총 일수
    int currentDayDisplay = 1;

    qDebug() << "Month Details: FirstDayOfWeek=" << startDayOfWeek << "DaysInMonth=" << daysInMonth;

    // 6x7개의 모든 달력 셀을 순회
    for (int row = 1; row <= 6; ++row) {
        for (int col = 0; col < 7; ++col) {
            QString dateTextLabelName = QString("lbl_%1_%2").arg(row).arg(col);
            QLabel *dateTextLabel = this->findChild<QLabel *>(dateTextLabelName);
            QString dayButtonName = QString("btn_%1_%2").arg(row).arg(col);
            QPushButton *dayButton = this->findChild<QPushButton*>(dayButtonName);

            if (!dateTextLabel && !dayButton) continue;

            // 현재 달에 속하지 않는 칸들을 비활성화하고 비움
            if ((row == 1 && col < startDayOfWeek - 1) || currentDayDisplay > daysInMonth) {
                // 비어있는 칸 처리 - 현재 달이 아닌 날짜들
                if (dateTextLabel) dateTextLabel->setText("");
                if (dayButton) {
                    dayButton->setText("");           // 텍스트 비우기
                    dayButton->setEnabled(false);     // 버튼 비활성화
                    dayButton->setProperty("dateValue", QVariant()); // 버튼에 저장된 날짜 속성 제거
                }
            } else {
                // 현재 달의 유효한 날짜들을 처리
                QDate cellDate(currentDate.year(), currentDate.month(), currentDayDisplay);
                if (dateTextLabel) {
                    dateTextLabel->setText(QString::number(currentDayDisplay));
                }

                if (dayButton) {
                    dayButton->setEnabled(true);
                    // 버튼에 해당 날짜 정보를 저장해둠 (나중에 클릭 시 사용)
                    dayButton->setProperty("dateValue", cellDate);
                    dayButton->setStyleSheet("");

                    QStringList logsDisplayForButton;

                    // 표시하도록 선택된 직원이 있을 경우에만 실행
                    if (m_dataManager && !m_checkedEmployeeIdsForDisplay.isEmpty()) {
                        // 선택된 모든 직원에 대해 반복
                        for (int employeeId : m_checkedEmployeeIdsForDisplay) {
                            Employee emp = m_dataManager->getEmployeeById(employeeId);
                            if (emp.getId() == -1 || emp.getName().isEmpty()) continue;

                            // 해당 직원의 해당 날짜 근무 기록을 데이터 관리자로부터 가져옴
                            QList<WorkLog> logs = m_dataManager->getWorkLogsForEmployeeOnDate(employeeId, cellDate);
                            if (logs.isEmpty()) continue;

                            // 근무 기록들을 요약하여 하나의 문자열로 만듦
                            // ... (요약 로직) ...
                            double dailyTotalHoursForEmp = 0;
                            QTime earliestStartForEmp = QTime(23,59,59);
                            QTime latestEndForEmp = QTime(0,0,0);
                            bool workedThisDay = false;
                            for(const WorkLog& log : logs) {
                                dailyTotalHoursForEmp += log.getHoursWorked();
                                if(log.getStartTime().isValid() && log.getStartTime() < earliestStartForEmp) earliestStartForEmp = log.getStartTime();
                                if(log.getEndTime().isValid() && log.getEndTime() > latestEndForEmp) latestEndForEmp = log.getEndTime();
                                workedThisDay = true;
                            }


                            if (workedThisDay && dailyTotalHoursForEmp > 0) {
                                QString startTimeStr = earliestStartForEmp.toString("HH:mm");
                                QString endTimeStr = latestEndForEmp.toString("HH:mm");
                                QString totalHoursStr = QString::number(dailyTotalHoursForEmp, 'f', 2) + "h";

                                QString logEntry = QString("%1: %2~%3 / %4")
                                                       .arg(emp.getName())
                                                       .arg(startTimeStr)
                                                       .arg(endTimeStr)
                                                       .arg(totalHoursStr);
                                logsDisplayForButton.append(logEntry);
                            }
                        }
                    }
                    // 요약된 모든 근무 기록 텍스트를 버튼에 표시
                    QString buttonDisplayText = logsDisplayForButton.join("\n");
                    dayButton->setText(buttonDisplayText);
                    if (buttonDisplayText.isEmpty()) { dayButton->setStyleSheet(""); }
                }
                currentDayDisplay++;
            }
        }
    }
    qDebug() << "--- CalendarWidget::updateCalendar() FINISHED ---";

}

// '이전 달' 버튼 클릭 시
void CalendarWidget::showPreviousMonth()
{
    currentDate = currentDate.addMonths(-1);
    updateCalendar(); // 달력을 새로 그림
}

// '다음 달' 버튼 클릭 시
void CalendarWidget::showNextMonth()
{
    currentDate = currentDate.addMonths(1);
    updateCalendar(); // 달력을 새로 그림
}


// 달력의 날짜 버튼이 클릭되었을 때 실행
void CalendarWidget::onDayButtonClicked()
{
    // 어떤 버튼이 클릭되었는지 Qt의 sender() 함수로 확인
    QPushButton *clickedButton = qobject_cast<QPushButton*>(sender());
    if (clickedButton) {
        // 버튼에 저장해두었던 날짜(QDate) 정보를 가져옴
        QVariant dateProperty = clickedButton->property("dateValue");
        if (dateProperty.isValid() && dateProperty.canConvert<QDate>()) {
            QDate clickedDate = dateProperty.toDate();
            if (clickedDate.isValid()) {
                // 유효한 날짜인 경우, MainWindow에 날짜가 클릭되었음을 알리는 신호를 보냄
                emit dateClicked(clickedDate); // dateClicked 시그널 발생
            }
        }
    }
}

// calendarwidget.cpp 에 추가

void CalendarWidget::updateDayCellDisplay(const QDate &date, int employeeIndex)
{
    if (!m_dataManager || !date.isValid() || employeeIndex < 0) {
        return;
    }

    QPushButton *targetButton = nullptr;
    // 특정 날짜에 해당하는 버튼을 찾음
    for (int r = 1; r <= 6; ++r) {
        for (int c = 0; c < 7; ++c) {
            QString btnName = QString("btn_%1_%2").arg(r).arg(c);
            QPushButton *btn = findChild<QPushButton*>(btnName);
            if (btn) {
                QVariant dateProp = btn->property("dateValue");
                if (dateProp.isValid() && dateProp.toDate() == date) {
                    targetButton = btn;
                    break;
                }
            }
        }
        if (targetButton) break;
    }

    // 찾은 버튼의 텍스트를 근무 시간으로 업데이트
    if (targetButton) {
        QList<WorkLog> logs = m_dataManager->getWorkLogsForEmployeeOnDate(employeeIndex, date);
        double totalHours = 0;
        for (const WorkLog &log : logs) {
            totalHours += log.getHoursWorked(); // WorkLog의 getHoursWorked() 사용
        }
        if (totalHours > 0) {
            targetButton->setText(QString("%1h").arg(totalHours));
        } else {
            targetButton->setText("");
        }
    }
}

// 달력에 표시할 직원 ID 목록을 설정하는 함수
void CalendarWidget::setCheckedEmployeesForDisplay(const QList<int>& checkedIds)
{
    qDebug() << "==> CalendarWidget::setCheckedEmployeesForDisplay - Setting IDs:" << checkedIds;
    // 이 목록은 updateCalendar 함수에서 사용됨
    m_checkedEmployeeIdsForDisplay = checkedIds;
    qDebug() << "    CalendarWidget: m_checkedEmployeeIdsForDisplay is NOW:" << m_checkedEmployeeIdsForDisplay;
}

// 달력 표시를 새로고침하는 외부 호출용 함수
void CalendarWidget::refreshDisplay()
{
    updateCalendar(); // 실제로는 updateCalendar 함수가 모든 표시 로직을 담당
}
