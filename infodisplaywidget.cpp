#include "infodisplaywidget.h"
#include "employee.h"
#include "worklog.h"
#include <QDebug>
#include <QLocale>

// 생성자: 시작일과 종료일을 현재 월로 초기화하고 UI를 구성
InfoDisplayWidget::InfoDisplayWidget(DataManager* dataManager, QWidget *parent)
    : QWidget(parent), m_dataManager(dataManager)
{
    QFont font = this->font();
    font.setPointSize(14);  // 폰트 크기 키움
    this->setFont(font);

    QDate today = QDate::currentDate();
    m_startDate = QDate(today.year(), today.month(), 1);
    m_endDate = QDate(today.year(), today.month(), today.daysInMonth());

    setupUI();
    refreshEmployeeTabs();
}

// UI 구성 함수: 기간 선택 및 탭 위젯 생성
void InfoDisplayWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 기간 선택 위젯 생성
    QGroupBox* periodGroup = new QGroupBox("기간 선택");
    QHBoxLayout* periodLayout = new QHBoxLayout(periodGroup);

    m_startDateEdit = new QDateEdit(m_startDate);
    m_startDateEdit->setCalendarPopup(true);

    m_endDateEdit = new QDateEdit(m_endDate);
    m_endDateEdit->setCalendarPopup(true);

    m_updateButton = new QPushButton("갱신");

    periodLayout->addWidget(new QLabel("시작일:"));
    periodLayout->addWidget(m_startDateEdit);
    periodLayout->addWidget(new QLabel("종료일:"));
    periodLayout->addWidget(m_endDateEdit);
    periodLayout->addWidget(m_updateButton);
    periodLayout->addStretch();

    connect(m_updateButton, &QPushButton::clicked, this, &InfoDisplayWidget::onPeriodChanged);

    // 직원별 정보 및 집계 탭
    m_tabWidget = new QTabWidget();

    mainLayout->addWidget(periodGroup);
    mainLayout->addWidget(m_tabWidget);
}

// 직원별 탭과 집계 탭 생성 및 초기화
void InfoDisplayWidget::refreshEmployeeTabs()
{
    m_tabWidget->clear();  // 기존 탭 제거
    m_employeeTabWidgets.clear();

    const QList<Employee>& employees = m_dataManager->getEmployees();

    // 직원별 탭 추가
    for (const Employee& emp : employees) {
        QWidget* employeeTab = createEmployeeTab(emp.getId());
        m_tabWidget->addTab(employeeTab, emp.getName());
    }

    // 집계 탭 추가
    QWidget* aggregateTab = createAggregateTab();
    m_tabWidget->addTab(aggregateTab, "집계");

    updateAllTabs();
}

// 직원별 개별 탭 생성
QWidget* InfoDisplayWidget::createEmployeeTab(int employeeId)
{
    QWidget* tab = new QWidget();
    QGridLayout* layout = new QGridLayout(tab);

    EmployeeTabWidgets widgets;

    widgets.hourlyWageLabel = new QLabel("시급: -");
    widgets.workHoursLabel = new QLabel("근무시간: -");
    widgets.basicPayLabel = new QLabel("근무시간 급여: -");
    widgets.weeklyHolidayLabel = new QLabel("+ 주휴수당: -");
    widgets.taxLabel = new QLabel("- 세금: -");
    widgets.totalPayLabel = new QLabel("= 총급여: -");

    // 구분선 생성
    QFrame* separator1 = new QFrame();
    separator1->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    QFrame* separator2 = new QFrame();
    separator2->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    // 레이아웃에 위젯 배치
    layout->addWidget(widgets.hourlyWageLabel, 1, 0);
    layout->addWidget(widgets.workHoursLabel, 2, 0);
    layout->addWidget(separator1, 3, 0);
    layout->addWidget(widgets.basicPayLabel, 4, 0);
    layout->addWidget(widgets.weeklyHolidayLabel, 5, 0);
    layout->addWidget(widgets.taxLabel, 6, 0);
    layout->addWidget(separator2, 7, 0);
    layout->addWidget(widgets.totalPayLabel, 8, 0);
    layout->setRowStretch(9, 1);

    // 저장
    m_employeeTabWidgets[employeeId] = widgets;

    return tab;
}

// 집계 탭 생성
QWidget* InfoDisplayWidget::createAggregateTab()
{
    QWidget* tab = new QWidget();
    QGridLayout* layout = new QGridLayout(tab);

    m_selectedEmployeesLabel = new QLabel("선택된 직원: 없음");
    m_selectedEmployeesLabel->setWordWrap(true);
    m_selectedEmployeesLabel->setStyleSheet("color: #333; font-weight: bold;");
    m_aggBasicPayLabel = new QLabel("근무시간 급여: -");
    m_aggWeeklyHolidayLabel = new QLabel("+ 주휴수당: -");
    m_aggTaxLabel = new QLabel("- 세금: -");
    m_aggTotalPayLabel = new QLabel("= 총급여: -");

    QFrame* separator1 = new QFrame();
    separator1->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    QFrame* separator2 = new QFrame();
    separator2->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    layout->addWidget(new QLabel("집계"), 0, 0);
    layout->addWidget(m_selectedEmployeesLabel, 1, 0);
    layout->addWidget(separator1, 2, 0);
    layout->addWidget(m_aggBasicPayLabel, 3, 0);
    layout->addWidget(m_aggWeeklyHolidayLabel, 4, 0);
    layout->addWidget(m_aggTaxLabel, 5, 0);
    layout->addWidget(separator2, 6, 0);
    layout->addWidget(m_aggTotalPayLabel, 7, 0);
    layout->setRowStretch(8, 1);
    return tab;
}

// 외부에서 선택된 직원 ID 설정
void InfoDisplayWidget::updateSelectedEmployees(const QList<int>& employeeIds)
{
    m_selectedEmployeeIds = employeeIds;
    updateAggregateTab();
}

// 기간 변경 시 호출
void InfoDisplayWidget::onPeriodChanged()
{
    m_startDate = m_startDateEdit->date();
    m_endDate = m_endDateEdit->date();
    updateAllTabs();
}

// 전체 탭 업데이트
void InfoDisplayWidget::updateAllTabs()
{
    for (auto it = m_employeeTabWidgets.begin(); it != m_employeeTabWidgets.end(); ++it) {
        int employeeId = it.key();
        updateEmployeeTab(employeeId);
    }

    updateAggregateTab();
}

// 특정 직원의 탭 업데이트
void InfoDisplayWidget::updateEmployeeTab(int employeeId)
{
    if (!m_employeeTabWidgets.contains(employeeId)) return;

    Employee emp = m_dataManager->getEmployeeById(employeeId);
    if (emp.getId() == -1) return;

    EmployeeTabWidgets& widgets = m_employeeTabWidgets[employeeId];

    double totalHours = calculateTotalHours(employeeId, m_startDate, m_endDate);
    double basicPay = totalHours * emp.getHourlyWage();
    double weeklyHoliday = calculateWeeklyHolidayPay(employeeId, m_startDate, m_endDate);
    double tax = (basicPay + weeklyHoliday) * 0.033;
    double totalPay = basicPay + weeklyHoliday - tax;

    QLocale locale(QLocale::Korean); // 원화 표시
    widgets.hourlyWageLabel->setText("시급: " + locale.toString(emp.getHourlyWage()) + "원");
    widgets.workHoursLabel->setText(QString("근무시간: %1시간").arg(totalHours, 0, 'f', 1));
    widgets.basicPayLabel->setText("근무시간 급여: " + locale.toString((int)basicPay) + "원");
    widgets.weeklyHolidayLabel->setText("+ 주휴수당: " + locale.toString((int)weeklyHoliday) + "원");
    widgets.taxLabel->setText("- 세금: " + locale.toString((int)tax) + "원");
    widgets.totalPayLabel->setText("= 총급여: " + locale.toString((int)totalPay) + "원");
}

// 집계 탭 업데이트
void InfoDisplayWidget::updateAggregateTab()
{
    if (m_selectedEmployeeIds.isEmpty()) {
        m_selectedEmployeesLabel->setText("선택된 직원: 없음");
        m_aggBasicPayLabel->setText("근무시간 급여: 직원을 선택해주세요");
        m_aggWeeklyHolidayLabel->setText("+ 주휴수당: -");
        m_aggTaxLabel->setText("- 세금: -");
        m_aggTotalPayLabel->setText("= 총급여: -");
        return;
    }

    QStringList employeeNames;
    for (int employeeId : m_selectedEmployeeIds) {
        Employee emp = m_dataManager->getEmployeeById(employeeId);
        if (emp.getId() != -1) {
            employeeNames << emp.getName();
        }
    }

    QString selectedText;
    if (employeeNames.size() == 1) {
        selectedText = QString("선택된 직원: %1").arg(employeeNames.first());
    } else {
        selectedText = QString("선택된 직원 (%1명): %2")
                           .arg(employeeNames.size())
                           .arg(employeeNames.join(", "));
    }
    m_selectedEmployeesLabel->setText(selectedText);

    double totalBasicPay = 0, totalWeeklyHoliday = 0, totalTax = 0;

    // 선택된 직원들에 대한 합계 계산
    for (int employeeId : m_selectedEmployeeIds) {
        Employee emp = m_dataManager->getEmployeeById(employeeId);
        if (emp.getId() == -1) continue;

        double hours = calculateTotalHours(employeeId, m_startDate, m_endDate);
        double basicPay = hours * emp.getHourlyWage();

        totalBasicPay += basicPay;
        totalWeeklyHoliday += calculateWeeklyHolidayPay(employeeId, m_startDate, m_endDate);
    }

    totalTax = (totalBasicPay + totalWeeklyHoliday) * 0.033;
    double totalPay = totalBasicPay + totalWeeklyHoliday - totalTax;

    QLocale locale(QLocale::Korean);
    m_aggBasicPayLabel->setText("근무시간 급여: " + locale.toString((int)totalBasicPay) + "원");
    m_aggWeeklyHolidayLabel->setText("+ 주휴수당: " + locale.toString((int)totalWeeklyHoliday) + "원");
    m_aggTaxLabel->setText("- 세금: " + locale.toString((int)totalTax) + "원");
    m_aggTotalPayLabel->setText("= 총급여: " + locale.toString((int)totalPay) + "원");
}

// 특정 직원의 전체 근무 시간 계산
double InfoDisplayWidget::calculateTotalHours(int employeeId, const QDate& startDate, const QDate& endDate)
{
    double totalHours = 0.0;
    const QList<WorkLog>& workLogs = m_dataManager->getWorkLogs();

    for (const WorkLog& log : workLogs) {
        if (log.getEmployeeId() == employeeId &&
            log.getDate() >= startDate &&
            log.getDate() <= endDate) {
            totalHours += log.getHoursWorked();
        }
    }

    return totalHours;
}

// 주별 근무시간을 QMap 형태로 계산 (월요일 시작 기준)
QMap<QDate, double> InfoDisplayWidget::calculateWeeklyHours(int employeeId, const QDate& startDate, const QDate& endDate)
{
    QMap<QDate, double> weeklyHours;
    QDate currentDate = startDate;

    while (currentDate <= endDate) {
        WorkLog log = m_dataManager->getWorkLogByEmployeeAndDate(employeeId, currentDate);
        if (log.getEmployeeId() != -1) {
            QDate weekStart = currentDate.addDays(1 - currentDate.dayOfWeek()); // 월요일 기준
            weeklyHours[weekStart] += log.getHoursWorked();
        }
        currentDate = currentDate.addDays(1);
    }

    return weeklyHours;
}

// 주휴수당 계산: 주당 15시간 이상 근무 시, 시급 * 0.2 * 그 주의 시간
double InfoDisplayWidget::calculateWeeklyHolidayPay(int employeeId, const QDate& startDate, const QDate& endDate)
{
    Employee emp = m_dataManager->getEmployeeById(employeeId);
    if (emp.getId() == -1) return 0.0;

    QMap<QDate, double> weeklyHours = calculateWeeklyHours(employeeId, startDate, endDate);
    double totalWeeklyHoliday = 0.0;

    for (auto it = weeklyHours.begin(); it != weeklyHours.end(); ++it) {
        double weekHours = it.value();
        if (weekHours >= 15.0) {
            totalWeeklyHoliday += emp.getHourlyWage() * 0.2 * weekHours;
        }
    }

    return totalWeeklyHoliday;
}
