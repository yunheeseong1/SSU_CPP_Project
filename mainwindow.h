#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDate>

// 주요 위젯 클래스들을 미리 선언 (전방 선언)
class CalendarWidget;
class EmployeePanelWidget;
class DataManager;
class QHBoxLayout;
class QVBoxLayout;
class InfoDisplayWidget;


namespace Ui {
class MainWindow;
}

// 프로그램의 전체 창 (모든 위젯들을 조립하고 관리하는 역할)
class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    // 프로그램 창이 닫히기 직전에 자동으로 실행되는 함수
    void closeEvent(QCloseEvent *event) override;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 달력에서 날짜를 클릭했을 때 실행
    void onCalendarDateClicked(const QDate &date);
    // 직원 목록에서 체크된 직원이 바뀌었을 때 실행
    void onCheckedEmployeesChanged(const QList<int>& checkedIndices);

private:
    Ui::MainWindow *ui; // UI 요소 관리 포인터

    // 메인 윈도우에 포함된 주요 위젯 및 데이터 관리자
    CalendarWidget *m_calendarWidget;
    EmployeePanelWidget *m_employeePanelWidget;
    DataManager *m_dataManager;
    InfoDisplayWidget* m_infoDisplayWidget;

    // 레이아웃 관리를 위한 멤버
    QWidget *m_centralArea;
    QHBoxLayout *m_mainAppLayout;
    QVBoxLayout *m_rightSideLayout;
};

#endif // MAINWINDOW_H
