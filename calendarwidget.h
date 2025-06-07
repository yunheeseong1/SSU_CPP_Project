#ifndef CALENDARWIDGET_H
#define CALENDARWIDGET_H

#include <QWidget>
#include <QDate>
#include <QList>
#include "datamanager.h"

class QPushButton;

namespace Ui {
class CalendarWidget;
}

// 월별 달력을 표시하고 근무 기록을 보여주는 위젯 클래스
class CalendarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CalendarWidget(DataManager *dataManager, QWidget *parent = nullptr);
    ~CalendarWidget();

    void updateDayCellDisplay(const QDate &date, int employeeId);
    // 달력 UI를 최신 데이터로 새로고침
    void updateCalendar();

    // 달력에 표시할 직원 목록을 설정
    void setCheckedEmployeesForDisplay(const QList<int>& checkedIds);
    // 달력 화면을 다시 그림
    void refreshDisplay();

signals:
    // 사용자가 날짜를 클릭했을 때 발생하는 신호
    void dateClicked(const QDate &date);

private slots:
    // '이전 달' 버튼 클릭 시 실행
    void showPreviousMonth();
    // '다음 달' 버튼 클릭 시 실행
    void showNextMonth();
    // 달력의 날짜(버튼)를 클릭했을 때 실행
    void onDayButtonClicked();

private:
    Ui::CalendarWidget *ui; // UI 요소 관리 포인터
    QDate currentDate; // 현재 달력이 보여주는 기준 날짜
    DataManager *m_dataManager; // 데이터 관리자 포인터
    QList<int> m_checkedEmployeeIdsForDisplay; // 화면에 표시할 직원 ID 목록
};

#endif // CALENDARWIDGET_H

