#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "calendarwidget.h"
#include "employeepanelwidget.h"
#include "infodisplaywidget.h"
#include "inputworkhoursdialog.h"
#include "datamanager.h"
#include <QMessageBox>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCloseEvent>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_calendarWidget(nullptr)
    , m_employeePanelWidget(nullptr)
    , m_dataManager(nullptr)
    , m_centralArea(nullptr)
    , m_mainAppLayout(nullptr)
    , m_rightSideLayout(nullptr)
    , m_infoDisplayWidget(nullptr)
{
    ui->setupUi(this);
    m_dataManager = new DataManager();

    m_centralArea = new QWidget(this);
    setCentralWidget(m_centralArea);

    // 초기 레이아웃 삭제 후 세로 레이아웃으로 설정
    delete m_mainAppLayout;
    QVBoxLayout* mainVerticalLayout = new QVBoxLayout(m_centralArea);
    m_mainAppLayout = nullptr;

    // 주요 위젯 생성
    m_calendarWidget = new CalendarWidget(m_dataManager, m_centralArea);
    m_employeePanelWidget = new EmployeePanelWidget(m_dataManager, m_centralArea);
    m_infoDisplayWidget = new InfoDisplayWidget(m_dataManager, m_centralArea);

    // 상단: 달력 + 직원 패널
    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addWidget(m_calendarWidget, 3);

    QVBoxLayout* rightSideLayout = new QVBoxLayout();
    rightSideLayout->addStretch(0);
    rightSideLayout->addWidget(m_employeePanelWidget, 3);
    topLayout->addLayout(rightSideLayout, 1);

    // 메인 레이아웃 구성
    mainVerticalLayout->addLayout(topLayout, 8);
    mainVerticalLayout->addWidget(m_infoDisplayWidget, 3);

    // 시그널-슬롯 연결
    if (m_calendarWidget) {
        connect(m_calendarWidget, &CalendarWidget::dateClicked, this, &MainWindow::onCalendarDateClicked);
    }

    if (m_employeePanelWidget) {
        connect(m_employeePanelWidget, &EmployeePanelWidget::checkedEmployeesChanged,
                this, &MainWindow::onCheckedEmployeesChanged);
        connect(m_employeePanelWidget, &EmployeePanelWidget::checkedEmployeesChanged,
                m_infoDisplayWidget, &InfoDisplayWidget::updateSelectedEmployees);
        connect(m_employeePanelWidget, &EmployeePanelWidget::employeeListChanged,
                m_infoDisplayWidget, &InfoDisplayWidget::refreshEmployeeTabs);
    }

    // 데이터 로드 및 UI 초기화
    QString dataFilePath = "salary_data.json";
    if (m_dataManager->loadData(dataFilePath)) {
        qDebug() << "Data loaded successfully from" << dataFilePath;
        m_employeePanelWidget->refreshEmployeeList();
        m_calendarWidget->setCheckedEmployeesForDisplay(m_employeePanelWidget->getCheckedEmployeeIds());
        m_calendarWidget->refreshDisplay();
        m_infoDisplayWidget->refreshEmployeeTabs();
        m_infoDisplayWidget->updateSelectedEmployees(QList<int>());
    } else {
        qWarning() << "Failed to load data. Starting with empty UI.";
        m_employeePanelWidget->refreshEmployeeList();
        m_calendarWidget->setCheckedEmployeesForDisplay(QList<int>());
        m_calendarWidget->refreshDisplay();
        m_infoDisplayWidget->refreshEmployeeTabs();
        m_infoDisplayWidget->updateSelectedEmployees(QList<int>());
    }

    setWindowTitle("알바 월급 프로그램");
    resize(1500, 900);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_dataManager;
}

// 날짜 클릭 시 근무시간 입력 다이얼로그 호출
void MainWindow::onCalendarDateClicked(const QDate &date)
{
    qDebug() << "MainWindow: Date clicked -" << date.toString("yyyy-MM-dd");

    if (!m_employeePanelWidget || !m_dataManager) return;

    QList<int> checkedIds = m_employeePanelWidget->getCheckedEmployeeIds();
    if (checkedIds.size() != 1) {
        QMessageBox::information(this, "알림", checkedIds.isEmpty() ?
                                                   "근무 시간을 입력할 직원을 먼저 체크해주세요." :
                                                   "근무 시간을 입력할 직원 한 명만 체크해주세요.");
        return;
    }

    int employeeId = checkedIds.first();
    Employee selectedEmployee = m_dataManager->getEmployeeById(employeeId);
    if (selectedEmployee.getId() == -1) {
        QMessageBox::warning(this, "오류", QString("직원 정보(ID: %1)를 가져올 수 없습니다.").arg(employeeId));
        return;
    }

    WorkLog existingLog = m_dataManager->getWorkLogByEmployeeAndDate(employeeId, date);
    bool hasExistingLog = (existingLog.getEmployeeId() != -1);

    InputWorkHoursDialog* dialog = hasExistingLog ?
                                       new InputWorkHoursDialog(selectedEmployee.getName(), employeeId, date, existingLog, this) :
                                       new InputWorkHoursDialog(selectedEmployee.getName(), employeeId, date, this);

    if (dialog->exec() == QDialog::Accepted) {
        if (dialog->isDeleteRequested()) {
            if (hasExistingLog && m_dataManager->deleteWorkLog(employeeId, date)) {
                QMessageBox::information(this, "완료", "근무 기록이 삭제되었습니다.");
            } else {
                QMessageBox::warning(this, "오류", "근무 기록 삭제에 실패했습니다.");
            }
        } else {
            WorkLog newLog = dialog->getWorkLog();
            if (hasExistingLog) {
                if (m_dataManager->updateWorkLog(existingLog, newLog)) {
                    QMessageBox::information(this, "완료", "근무 기록이 수정되었습니다.");
                } else {
                    QMessageBox::warning(this, "오류", "근무 기록 수정에 실패했습니다.");
                }
            } else {
                m_dataManager->addWorkLog(newLog);
                QMessageBox::information(this, "완료", "근무 기록이 추가되었습니다.");
            }
        }

        // UI 갱신
        m_calendarWidget->refreshDisplay();
        m_infoDisplayWidget->updateAllTabs();
    }

    delete dialog;
}

// 체크된 직원 변경 시 달력 갱신
void MainWindow::onCheckedEmployeesChanged(const QList<int>& checkedIds)
{
    qDebug() << "==> MainWindow::onCheckedEmployeesChanged - Received IDs:" << checkedIds;
    m_calendarWidget->setCheckedEmployeesForDisplay(checkedIds);
    m_calendarWidget->refreshDisplay();
}

// 종료 시 데이터 저장
void MainWindow::closeEvent(QCloseEvent *event)
{
    QString dataFilePath = "salary_data.json";
    if (!m_dataManager->saveData(dataFilePath)) {
        qWarning() << "데이터 저장에 실패했습니다.";
    }
    QMainWindow::closeEvent(event);
}
