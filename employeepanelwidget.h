#ifndef EMPLOYEEPANELWIDGET_H
#define EMPLOYEEPANELWIDGET_H

#include <QWidget>
#include <QList>
#include "datamanager.h"

class QListWidgetItem;

namespace Ui {
class EmployeePanelWidget;
}

// 직원 목록을 보여주고 관리(추가/수정/삭제)하는 위젯 클래스
class EmployeePanelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EmployeePanelWidget(DataManager *dataManager, QWidget *parent = nullptr);
    ~EmployeePanelWidget();

    // 직원 목록 UI를 최신 데이터로 새로고침
    void refreshEmployeeList();
    QList<int> getCheckedEmployeeIndices() const;
    // UI에서 현재 체크된 직원들의 ID 목록을 가져옴
    QList<int> getCheckedEmployeeIds() const;
    int getSelectedEmployeeIndex() const ;

signals:
    // 체크된 직원 목록이 변경될 때 발생하는 신호
    void checkedEmployeesChanged(const QList<int>& checkedIndices);
    // 직원 목록 자체가 변경(추가/삭제)될 때 발생하는 신호
    void employeeListChanged();

private slots:
    // '직원 추가' 버튼 클릭 시 실행
    void on_addEmployeeButton_clicked();
    // '정보 수정' 버튼 클릭 시 실행
    void on_editEmployeeButton_clicked();
    // '직원 삭제' 버튼 클릭 시 실행
    void on_deleteEmployeeButton_clicked();
    // 목록에서 아이템의 체크 상태가 바뀔 때 실행
    void on_employeeListWidget_itemChanged(QListWidgetItem *item);

private:
    Ui::EmployeePanelWidget *ui; // UI 요소 관리 포인터
    DataManager *m_dataManager; // 데이터 관리자 포인터
    // 버튼 활성화/비활성화 상태 업데이트
    void updateButtonStates();
};

#endif // EMPLOYEEPANELWIDGET_H
