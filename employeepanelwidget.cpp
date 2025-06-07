#include "employeepanelwidget.h"
#include "ui_employeepanelwidget.h"
#include "addemployeedialog.h"
#include "employee.h"
#include <QListWidgetItem>
#include <QMessageBox>
#include <QDebug>     // 디버깅용 출력

// 생성자: UI 설정 및 시그널-슬롯 연결
EmployeePanelWidget::EmployeePanelWidget(DataManager *dataManager, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EmployeePanelWidget),
    m_dataManager(dataManager)
{
    ui->setupUi(this);

    // 직원 체크박스 상태가 바뀔 때 버튼 상태를 갱신
    connect(ui->employeeListWidget, &QListWidget::itemChanged,
            this, &EmployeePanelWidget::on_employeeListWidget_itemChanged);

    updateButtonStates();      // 버튼 활성화/비활성화 상태 초기화
    refreshEmployeeList();     // 직원 목록 로드
}

// 소멸자: UI 리소스 해제
EmployeePanelWidget::~EmployeePanelWidget()
{
    delete ui;
}

// "직원 추가" 버튼 클릭 시 실행
void EmployeePanelWidget::on_addEmployeeButton_clicked()
{
    if (!m_dataManager) return;

    AddEmployeeDialog dialog(this); // 직원 추가 다이얼로그 실행
    if (dialog.exec() == QDialog::Accepted) {
        // 입력된 정보로 새 직원 데이터 생성
        Employee newEmployeeData = dialog.getEmployeeDetails();

        // DataManager에 직원 추가
        m_dataManager->addEmployee(newEmployeeData);

        // 목록 새로고침 및 변경 시그널 발생
        refreshEmployeeList();
        emit employeeListChanged();
    }
}

// "직원 수정" 버튼 클릭 시 실행
void EmployeePanelWidget::on_editEmployeeButton_clicked()
{
    if (!m_dataManager) return;

    QList<int> checkedIds = getCheckedEmployeeIds(); // 체크된 직원들의 ID 가져오기

    // 정확히 1명만 선택되어야 수정 가능
    if (checkedIds.size() != 1) {
        QMessageBox::information(this, "알림", "수정할 직원 한 명만 체크해주세요.");
        return;
    }

    int employeeIdToEdit = checkedIds.first(); // 수정할 직원 ID

    // ID로 직원 정보 가져오기
    Employee employeeToEdit = m_dataManager->getEmployeeById(employeeIdToEdit);
    if (employeeToEdit.getId() == -1) {
        QMessageBox::warning(this, "오류", QString("직원 정보(ID: %1)를 가져오는 데 실패했습니다.").arg(employeeIdToEdit));
        return;
    }

    // 다이얼로그에 기존 정보 채워넣기
    AddEmployeeDialog dialog(employeeToEdit, this);

    if (dialog.exec() == QDialog::Accepted) {
        Employee updatedEmployeeData = dialog.getEmployeeDetails(); // 수정된 정보

        // 직원 정보 업데이트 시도
        bool success = m_dataManager->updateEmployeeById(employeeIdToEdit, updatedEmployeeData);

        if (success) {
            refreshEmployeeList();
            emit employeeListChanged();
        } else {
            QMessageBox::warning(this, "수정 실패", "직원 정보 수정에 실패했습니다.");
        }
    }
}

// "직원 삭제" 버튼 클릭 시 실행
void EmployeePanelWidget::on_deleteEmployeeButton_clicked()
{
    if (!m_dataManager) return;

    QList<int> checkedIds = getCheckedEmployeeIds(); // 체크된 직원들의 ID 가져오기

    if (checkedIds.isEmpty()) {
        QMessageBox::information(this, "알림", "삭제할 직원을 하나 이상 체크해주세요.");
        return;
    }

    // 삭제 확인 다이얼로그 표시
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "직원 삭제 확인",
                                  QString("%1명의 직원을 정말로 삭제하시겠습니까?").arg(checkedIds.size()),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        bool allSucceeded = true;

        // 선택된 직원들을 하나씩 삭제
        for (int id : checkedIds) {
            if (!m_dataManager->deleteEmployeeById(id)) {
                allSucceeded = false;
                qWarning() << "Failed to delete employee with ID:" << id;
            }
        }

        // 일부 실패 시 경고
        if (!allSucceeded) {
            QMessageBox::warning(this, "삭제 오류", "일부 직원 정보 삭제에 실패했습니다.");
        }

        refreshEmployeeList();
        emit employeeListChanged();
    }
}

// 체크된 직원 수에 따라 버튼 활성화 상태 설정
void EmployeePanelWidget::updateButtonStates()
{
    QList<int> checkedIds = getCheckedEmployeeIds(); // 체크된 ID 리스트

    // 수정 버튼은 한 명만 선택된 경우에만 활성화
    ui->editEmployeeButton->setEnabled(checkedIds.size() == 1);
    // 삭제 버튼은 하나 이상 선택되면 활성화
    ui->deleteEmployeeButton->setEnabled(!checkedIds.isEmpty());

    qDebug() << "EmployeePanelWidget: Emitting checkedEmployeesChanged with IDs:" << checkedIds;
    emit checkedEmployeesChanged(checkedIds); // 시그널 발신 (예: 근무 기록 연동 등)
}

// 직원 목록 새로고침 (리스트를 재구성)
void EmployeePanelWidget::refreshEmployeeList()
{
    if (!m_dataManager) return;

    ui->employeeListWidget->clear(); // 기존 항목 제거
    const QList<Employee>& employees = m_dataManager->getEmployees(); // 전체 직원 목록 가져오기

    for (const Employee& emp : employees) {
        QListWidgetItem *item = new QListWidgetItem();

        item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // 체크 가능하게 설정
        item->setCheckState(Qt::Unchecked); // 초기 상태는 체크 안 됨

        item->setData(Qt::UserRole, emp.getId()); // UserRole에 직원 고유 ID 저장
        item->setText(emp.getName()); // 항목 텍스트로 이름 설정

        ui->employeeListWidget->addItem(item);
    }

    updateButtonStates(); // 버튼 상태 업데이트
}

// 체크된 직원들의 ID를 반환
QList<int> EmployeePanelWidget::getCheckedEmployeeIds() const
{
    QList<int> checkedIds;

    for (int i = 0; i < ui->employeeListWidget->count(); ++i) {
        QListWidgetItem *item = ui->employeeListWidget->item(i);
        if (item && item->checkState() == Qt::Checked) {
            bool ok;
            int id = item->data(Qt::UserRole).toInt(&ok); // 저장된 ID 가져오기
            if (ok) {
                checkedIds.append(id);
            }
        }
    }

    return checkedIds;
}

// 직원 리스트 아이템의 체크 상태가 바뀔 때 실행되는 슬롯
void EmployeePanelWidget::on_employeeListWidget_itemChanged(QListWidgetItem *item)
{
    Q_UNUSED(item); // item 변수는 사용하지 않음
    updateButtonStates(); // 버튼 상태 업데이트
}
