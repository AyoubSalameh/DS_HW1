#include "HiringCompany.h"

HiringCompany::HiringCompany(int CompanyID, int value)
: companyID(CompanyID), value(value), numOfEmployees(0) ,companyHighestEarner(0, 0),
  companyIDTree(), companySalaryTree()
{}

int HiringCompany::getCompanyID() const
{
    return this->companyID;
}
int HiringCompany::getCompanyValue() const
{
    return this->value;
}
int HiringCompany::getCompanyNumOfEmployees() const
{
    return this->numOfEmployees;
}
HighestEarner HiringCompany::getCompanyHighestEarner() const
{
    return this->companyHighestEarner;
}

void HiringCompany::increaseCompanyValue(int ValueIncrease)
{
    this->value += ValueIncrease;
}
void HiringCompany::increaseCompanyNumOfEmployees()
{
    this->numOfEmployees++;
}
void HiringCompany::decreaseCompanyNumOfEmployees()
{
    this->numOfEmployees--;
}
void HiringCompany::setCompanyNumOfEmployees(int num)
{
    this->numOfEmployees = num;
}

///might need to change here. add try and catch
CompanyRes HiringCompany::AddEmployee(int employeeID, int salary, int grade, Node<int, Employee> *IDTreeEmp,
                                      Node<SalaryTreeKey, Employee> *SalTreeEmp)
{
    try {
        Node<int, Employee> *found = companyIDTree.find(employeeID);
        if (found != nullptr)
            return COMPANY_FAILURE;
        Employee newEmp(employeeID, salary, grade, companyID,
                        IDTreeEmp, SalTreeEmp);
        SalaryTreeKey salKey(employeeID, salary);

        TreeRes res = companyIDTree.insert(employeeID, newEmp);
        if (res != TREE_SUCCESS)
            return COMPANY_FAILURE;

        try {
            res = companySalaryTree.insert(salKey, newEmp);
        }
        catch (std::bad_alloc& e) {
                companyIDTree.remove(employeeID);
                throw;
        }
    }
    catch (std::bad_alloc& e)
    {
        throw;
    }
    companyHighestEarner.compareAndSetSalaries(employeeID, salary);
    this->increaseCompanyNumOfEmployees();
    return COMPANY_SUCCESS;
}

CompanyRes HiringCompany::removeEmployee(int employeeID)
{
    try {
        Node<int, Employee> *found = companyIDTree.find(employeeID);
        if (found == nullptr)
            return COMPANY_FAILURE;
        int employeeSalary = found->getData()->getSalary();
        Employee emp = *found->getData();
        SalaryTreeKey salKeyToRemove(employeeID, employeeSalary);

        TreeRes res;

        res = companyIDTree.remove(employeeID);
        if (res != TREE_SUCCESS)
            return COMPANY_FAILURE;

        res = companySalaryTree.remove(salKeyToRemove);
        if (res != TREE_SUCCESS) {
            ///in case the removal didnt work, we insert him again
            companyIDTree.insert(employeeID, emp);
            return COMPANY_FAILURE;
        }

        ///now we need to update the highest earner
        decreaseCompanyNumOfEmployees();
        if (numOfEmployees == 0) {
            ///we dont care about the highest earner if the company doesnt hire anyone
            companyHighestEarner.setID(0);
            companyHighestEarner.setSalary(0);
            return COMPANY_SUCCESS;
        }
        else if (employeeID == companyHighestEarner.getID())
        {
            Node <SalaryTreeKey, Employee> *newHighestEarner = companySalaryTree.findMax();
            companyHighestEarner.setID(newHighestEarner->getKey()->getID());
            companyHighestEarner.setSalary(newHighestEarner->getKey()->getSalary());
        }
    }
    catch (std::bad_alloc& e)
    {
        throw;
    }
    return COMPANY_SUCCESS;
}

CompanyRes HiringCompany::promoteEmployee(int employeeID, int salaryIncrease, int bumpGrade,
                                                     Node<int,Employee>* newIDTreeEmp,
                                                     Node<SalaryTreeKey, Employee>* newSalTreeEmp)
{
    try
    {
        Node<int, Employee> *found = companyIDTree.find(employeeID);
        if (found == nullptr)
            return COMPANY_FAILURE;
        int newSalary = found->getData()->getSalary() + salaryIncrease;
        int newGrade = found->getData()->getGrade();
        if (bumpGrade > 0)
            newGrade++;
        CompanyRes res = removeEmployee(employeeID);
        if (res == COMPANY_FAILURE)
            return res;

        try {
           res = AddEmployee(employeeID, newSalary, newGrade,
                              newIDTreeEmp, newSalTreeEmp);
        }
             ///if it failed, we need to add employee again with old data
        catch (std::bad_alloc& e) {

                AddEmployee(employeeID, found->getData()->getSalary(), found->getData()->getGrade(),
                            found->getData()->getEmployeeInIDTree(),
                            found->getData()->getEmployeeInSalaryTree());
                throw;
        }
    }
    catch (std::bad_alloc& e)
    {
        throw;
    }
    return COMPANY_SUCCESS;
}

CompanyRes HiringCompany::fillArrayIDTreeData(int *keys, Employee *data)
{
    TreeRes res = companyIDTree.inOrderFillDataInArray(data);
    if(res == TREE_SUCCESS)
        return COMPANY_SUCCESS;
    return COMPANY_FAILURE;
}

CompanyRes HiringCompany::fillArrayReverseSalaryTreeKey(SalaryTreeKey *keys)
{
    TreeRes res = companySalaryTree.reverseInOrderFillKeyInArray(keys);
    if(res == TREE_SUCCESS)
        return COMPANY_SUCCESS;
    return COMPANY_FAILURE;
}


HiringCompany::HiringCompany(const HiringCompany &other) :
companyID(other.companyID), value(other.value), numOfEmployees(other.numOfEmployees),
companyHighestEarner(other.companyHighestEarner)
,companyIDTree(), companySalaryTree()
{
}

HiringCompany::HiringCompany() : companyID(0), value(0), numOfEmployees(0), companyHighestEarner(),
                                 companyIDTree(), companySalaryTree()

{}

CompanyRes
HiringCompany::GetNumMatchingInCompany(int minID, int maxID, int minSalary, int minGrade, int *TotalNumOfEmployees,
                                       int *NumOfEmployees)
{
    //when we call this function we know that we have workers in it
    *TotalNumOfEmployees = companyIDTree.findAmountInInterval(minID, maxID);

    Employee* arr = new Employee[*TotalNumOfEmployees];
    TreeRes res = companyIDTree.fillMatchingInArr(arr, minID, maxID);
    if(res != TREE_SUCCESS)
    {
        delete[] arr;
        return COMPANY_FAILURE;
    }

    int counter = 0;
    for(int i = 0; i < *TotalNumOfEmployees; i++)
    {
        if(arr[i].getGrade() >= minGrade && arr[i].getSalary() >= minSalary)
            counter++;
    }
    *NumOfEmployees = counter;
    delete[] arr;
    return COMPANY_SUCCESS;
}

void HiringCompany::setCompanyValue(int val)
{
    this->value = val;
}

CompanyRes HiringCompany::fillArrayKeyAndDataIDTree(int *keyArr, Employee *dataArr)
{
    TreeRes res = companyIDTree.inOrderFillKeyAndData(keyArr, dataArr);
    if(res != TREE_SUCCESS)
        return COMPANY_FAILURE;
    return COMPANY_SUCCESS;
}

CompanyRes HiringCompany::fillArrayKeyAndDataSalaryTree(SalaryTreeKey *keyArr, Employee *dataArr)
{
    TreeRes res = companySalaryTree.inOrderFillKeyAndData(keyArr, dataArr);
    if(res != TREE_SUCCESS)
        return COMPANY_FAILURE;
    return COMPANY_SUCCESS;
}

void HiringCompany::setCompanyHighestEarner(HighestEarner high)
{
    companyHighestEarner.setID(high.getID());
    companyHighestEarner.setSalary(high.getSalary());
}

CompanyRes HiringCompany::CreateIDTree(int *keyArr, Employee *dataArr, int size)
{

    //this->companyIDTree = Tree<int, Employee>();
    companyIDTree.ConvertArrayToTree(keyArr, dataArr, size);
    return COMPANY_SUCCESS;

}

CompanyRes HiringCompany::CreateSalaryTree(SalaryTreeKey *keyArr, Employee *dataArr, int size)
{
    //this->companySalaryTree = Tree<SalaryTreeKey, Employee>();
    companySalaryTree.ConvertArrayToTree(keyArr, dataArr, size);
    return COMPANY_SUCCESS;
}

int HiringCompany::operator()() {
    return this->companyHighestEarner.getID();
}







