#ifndef INFODISPLAYWIDGET_H
#define INFODISPLAYWIDGET_H

#include <QWidget>
#include <QTabWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QDateEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QFrame>
#include "datamanager.h"

// 직원별 급여 정보 탭에 들어가는 UI 라벨들을 묶어놓은 구조체
struct EmployeeTabWidgets {
    QLabel* nameLabel;
    QLabel* hourlyWageLabel;
    QLabel* workHoursLabel;
    QLabel* basicPayLabel;
    QLabel* weeklyHolidayLabel;
    QLabel* taxLabel;
    QLabel* totalPayLabel;
};

// 급여 정보를 계산해서 탭 형태로 보여주는 위젯 클래스
class InfoDisplayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit InfoDisplayWidget(DataManager* dataManager, QWidget *parent = nullptr);
    // 모든 탭의 내용을 최신 정보로 업데이트
    void updateAllTabs();
    // 직원 목록 변경 시 탭 자체를 새로 구성
    void refreshEmployeeTabs();

public slots:
    // 다른 위젯에서 선택된 직원 목록이 변경되었을 때 호출됨
    void updateSelectedEmployees(const QList<int>& employeeIds);

private slots:
    // 사용자가 '갱신' 버튼을 눌러 기간을 변경했을 때 호출됨
    void onPeriodChanged();

private:
    // private 헬퍼 함수들
    void setupUI(); // 위젯의 초기 UI를 설정
    void updateEmployeeTab(int employeeId); // 특정 직원 탭의 정보를 업데이트
    void updateAggregateTab(); // 집계 탭의 정보를 업데이트
    QWidget* createEmployeeTab(int employeeId); // 직원 탭 위젯 생성
    QWidget* createAggregateTab(); // 집계 탭 위젯 생성
    double calculateTotalHours(int employeeId, const QDate& startDate, const QDate& endDate); // 총 근무시간 계산
    double calculateWeeklyHolidayPay(int employeeId, const QDate& startDate, const QDate& endDate); // 주휴수당 계산

    // 멤버 변수
    DataManager* m_dataManager; // 데이터 관리자 포인터
    QTabWidget* m_tabWidget;    // 탭들을 담는 위젯

    // 기간 설정 UI 요소
    QDateEdit* m_startDateEdit;
    QDateEdit* m_endDateEdit;
    QPushButton* m_updateButton;

    // 집계 탭 UI 요소
    QLabel* m_selectedEmployeesLabel;
    QLabel* m_aggBasicPayLabel;
    QLabel* m_aggWeeklyHolidayLabel;
    QLabel* m_aggTaxLabel;
    QLabel* m_aggTotalPayLabel;

    // 직원 ID와 해당 직원의 탭 위젯들을 매핑하여 관리
    QMap<int, EmployeeTabWidgets> m_employeeTabWidgets;

    // 현재 선택된 직원 ID 목록과 날짜 기간
    QList<int> m_selectedEmployeeIds;
    QDate m_startDate;
    QDate m_endDate;

    QMap<QDate, double> calculateWeeklyHours(int employeeId, const QDate& startDate, const QDate& endDate);
};

#endif // INFODISPLAYWIDGET_H
