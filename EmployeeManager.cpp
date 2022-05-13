#include "EmployeeManager.h"
#include <memory>


EmployeeManager::EmployeeManager() : CompanyTree(), HiringCompaniesTree(), EmployeeIDTree(),
                                     EmployeeSalaryTree(), numOfHiringCompanies(0),
                                     totalEmployees(0), highestEarner()
{}

StatusType EmployeeManager::AddCompany(int CompanyID, int Value)
{
    try
    {
        if (CompanyID <= 0 || Value <= 0)
            return INVALID_INPUT;
        Node<int, int> *found = CompanyTree.find(CompanyID);
        if (found != nullptr)
            return FAILURE;
        TreeRes res = CompanyTree.insert(CompanyID, Value);
        if(res != TREE_SUCCESS)
            return FAILURE;
        return SUCCESS;
    }
    catch (std::bad_alloc& e)
    {
         return ALLOCATION_ERROR;
    }
}

StatusType EmployeeManager::AddEmployee(int EmployeeID, int CompanyID, int Salary, int Grade)
{

    try
    {
        if(EmployeeID <= 0 || CompanyID <= 0 || Salary <= 0 || Grade < 0)
            return INVALID_INPUT;
        Node<int, Employee>* foundEmployee = EmployeeIDTree.find(EmployeeID);
        if(foundEmployee != nullptr)
            return FAILURE;
        Node<int, int>* companyExists = CompanyTree.find(CompanyID);
        if(companyExists == nullptr)
            return FAILURE;

        int CompanyValue = *companyExists->getData();

        Employee newEmp(EmployeeID, Salary, Grade, CompanyID);
        SalaryTreeKey salTreeKey(EmployeeID, Salary);

        ///inserting to ID tree
        TreeRes res = EmployeeIDTree.insert(EmployeeID, newEmp);
        if(res != TREE_SUCCESS)
            return FAILURE;

        ///inserting to salary tree
        try {
            res = EmployeeSalaryTree.insert(salTreeKey, newEmp);
        } catch (std::bad_alloc& e) {
            EmployeeIDTree.remove(EmployeeID);
            throw;
        }

        Node<int, HiringCompany>* HiringCompanyExists = HiringCompaniesTree.find(CompanyID);

        ///inserting company to hiringCompaniesTree
        if(HiringCompanyExists == nullptr)   ///means the company has no workers, therefore we need to add her
        {
            HiringCompany newCompany(CompanyID, CompanyValue);
            try {
                res = HiringCompaniesTree.insert(CompanyID, newCompany);
            }
            catch (std::bad_alloc& e)
            {
                EmployeeIDTree.remove(EmployeeID);
                EmployeeSalaryTree.remove(salTreeKey);
                throw;
            }
            numOfHiringCompanies++;
        }

        Node<int, HiringCompany>* company = HiringCompaniesTree.find(CompanyID);
        Node<int, Employee>* empInIdTree = EmployeeIDTree.find(EmployeeID);
        Node<SalaryTreeKey, Employee>* empInSalTree = EmployeeSalaryTree.find(salTreeKey);

        ///inserting to company sub trees
        try {
            company->getData()->AddEmployee(EmployeeID, Salary, Grade,
                                            empInIdTree, empInSalTree);
        } catch (std::bad_alloc& e)
        {

            EmployeeIDTree.remove(EmployeeID);
            EmployeeSalaryTree.remove(salTreeKey);
            if(company->getData()->getCompanyNumOfEmployees() == 0)
            {

                HiringCompaniesTree.remove(CompanyID);
            }
            throw;
        }
    }
    catch (std::bad_alloc& e)
    {
        return ALLOCATION_ERROR;
    }

    totalEmployees++;
    highestEarner.compareAndSetSalaries(EmployeeID, Salary);
    return SUCCESS;
}

StatusType EmployeeManager::RemoveEmployee(int EmployeeID)
{

    if(EmployeeID <= 0)
        return INVALID_INPUT;

    Node<int, Employee>* employeeExists = EmployeeIDTree.find(EmployeeID);
    if(employeeExists == nullptr)
        return FAILURE;

    int salary = employeeExists->getData()->getSalary();
    int companyID = employeeExists->getData()->getCompanyID();

    Employee emp = *employeeExists->getData();

    SalaryTreeKey salTreeKey(EmployeeID, salary);

    TreeRes res;

    //removing him from ID tree
    res = EmployeeIDTree.remove(EmployeeID);
    if(res != TREE_SUCCESS)
        return FAILURE;

    //removing him from salary Tree
    ///this may be causing issues
    res = EmployeeSalaryTree.remove(salTreeKey);
    if(res != TREE_SUCCESS)

    {
        EmployeeIDTree.insert(EmployeeID, emp);
        return FAILURE;
    }

    Node<int, HiringCompany>* company = HiringCompaniesTree.find(companyID);
    if(company == nullptr)
        return FAILURE;

    //removing him from company tree
    HiringCompany comp = *company->getData();

    CompanyRes r = company->getData()->removeEmployee(EmployeeID);


    if(r != COMPANY_SUCCESS)
    {
        EmployeeIDTree.insert(EmployeeID, emp);
        EmployeeSalaryTree.insert(salTreeKey, emp);
        return FAILURE;
    }

    ///if the company is left with no workers
    if(company->getData()->getCompanyNumOfEmployees() == 0)
    {
        HiringCompaniesTree.remove(companyID);
        numOfHiringCompanies--;
    }

    totalEmployees--;
    if(totalEmployees == 0)
    {
        highestEarner.setID(0);
        highestEarner.setSalary(0);
    }
    else if(EmployeeID == highestEarner.getID())
    {
        Node<SalaryTreeKey, Employee>* newHighestEarner = EmployeeSalaryTree.findMax();
        highestEarner.setID(newHighestEarner->getKey()->getID());
        highestEarner.setSalary(newHighestEarner->getKey()->getSalary());
    }

    return SUCCESS;
}

StatusType EmployeeManager::RemoveCompany(int CompanyID)
{
    if(CompanyID <= 0)
        return INVALID_INPUT;

    Node<int, HiringCompany>* foundHiring = HiringCompaniesTree.find(CompanyID);
    if(foundHiring != nullptr)
        return FAILURE;

    Node<int, int>* exists = CompanyTree.find(CompanyID);
    if(exists == nullptr)
        return FAILURE;

    TreeRes res = CompanyTree.remove(CompanyID);
    if(res != TREE_SUCCESS)
        return FAILURE;

    return SUCCESS;
}

StatusType EmployeeManager::GetCompanyInfo(int CompanyID, int *Value, int *NumOfEmployees)
{
    if(CompanyID <= 0 || Value == nullptr || NumOfEmployees == nullptr)
        return INVALID_INPUT;

    Node<int, int>* exists = CompanyTree.find(CompanyID);
    if(exists == nullptr)
        return FAILURE;

    *Value = *exists->getData();
    *NumOfEmployees = 0;

    Node<int, HiringCompany>* foundHiring = HiringCompaniesTree.find(CompanyID);
    if(foundHiring != nullptr)
        *NumOfEmployees = foundHiring->getData()->getCompanyNumOfEmployees();

    return SUCCESS;
}

StatusType EmployeeManager::GetEmployeeInfo(int EmployeeID, int *EmployerID, int *Salary, int *Grade)
{
    if(EmployeeID <= 0 || EmployerID == nullptr || Salary == nullptr || Grade == nullptr)
        return INVALID_INPUT;

    Node<int, Employee>* found = EmployeeIDTree.find(EmployeeID);
    if(found == nullptr)
        return FAILURE;

    *EmployerID = found->getData()->getCompanyID();
    *Salary = found->getData()->getSalary();
    *Grade = found->getData()->getGrade();

    return SUCCESS;
}

StatusType EmployeeManager::IncreaseCompanyValue(int CompanyID, int ValueIncrease)
{
    if(CompanyID <= 0 || ValueIncrease <=0)
        return INVALID_INPUT;

    Node<int, int>* exist = CompanyTree.find(CompanyID);
    if(exist == nullptr)
        return FAILURE;

    *exist->getData() += ValueIncrease;

    Node<int, HiringCompany>* foundHiring = HiringCompaniesTree.find(CompanyID);
    if(foundHiring != nullptr)
        foundHiring->getData()->increaseCompanyValue(ValueIncrease);

    return SUCCESS;
}

StatusType EmployeeManager::PromoteEmployee(int EmployeeID, int SalaryIncrease, int BumpGrade)
{
    try
    {
        if(EmployeeID <= 0 || SalaryIncrease <= 0)
            return INVALID_INPUT;

        Node<int, Employee>* exist = EmployeeIDTree.find(EmployeeID);
        if(exist == nullptr)
            return FAILURE;

        int oldSalary = exist->getData()->getSalary();
        int oldGrade = exist->getData()->getGrade();
        int companyID = exist->getData()->getCompanyID();

        exist->getData()->increaseSalary(SalaryIncrease);
        exist->getData()->increaseGrade(BumpGrade);

        SalaryTreeKey oldSalTreeKey(EmployeeID, oldSalary);
        SalaryTreeKey newSalTreeKey(EmployeeID, oldSalary + SalaryIncrease);

        TreeRes res = EmployeeSalaryTree.remove(oldSalTreeKey);
        if(res != TREE_SUCCESS)
        {
            exist->getData()->setSalary(oldSalary);
            if(BumpGrade > 0)
                exist->getData()->setGrade(oldGrade);
            return FAILURE;
        }
        res = EmployeeSalaryTree.insert(newSalTreeKey, *exist->getData());
        if(res != TREE_SUCCESS)
        {
            exist->getData()->setSalary(oldSalary);
            if(BumpGrade > 0)
                exist->getData()->setGrade(oldGrade);
            EmployeeSalaryTree.insert(oldSalTreeKey,*exist->getData());
        }

        Node<SalaryTreeKey, Employee>* empInSalTree = EmployeeSalaryTree.find(newSalTreeKey);

        //now we find the company and update him there.
        Node<int, HiringCompany>* Company = HiringCompaniesTree.find(companyID);
        if(Company == nullptr)
            return FAILURE;

        Company->getData()->promoteEmployee(EmployeeID,SalaryIncrease,BumpGrade,
                                            exist, empInSalTree);

        highestEarner.compareAndSetSalaries(EmployeeID, oldSalary+ SalaryIncrease);

    }
    catch (std::bad_alloc& e)
    {
        return ALLOCATION_ERROR;
    }
    return SUCCESS;
}

StatusType EmployeeManager::HireEmployee(int EmployeeID, int NewCompanyID)
{
    if (EmployeeID <= 0 || NewCompanyID <= 0)
        return INVALID_INPUT;

    Node<int, Employee> *exist = EmployeeIDTree.find(EmployeeID);
    if (exist == nullptr)
        return FAILURE;

    Node<int, int>* companyExists = CompanyTree.find(NewCompanyID);
    if(companyExists == nullptr)
        return FAILURE;

    Employee emp = *exist->getData();
    if (emp.getCompanyID() == NewCompanyID)
        return FAILURE;

    int sal = emp.getSalary();
    int grade = emp.getGrade();

    StatusType res = RemoveEmployee(EmployeeID);
    if(res != SUCCESS)
        return res;


    res = AddEmployee(EmployeeID, NewCompanyID, sal, grade);
    if(res != SUCCESS)
    {
        AddEmployee(EmployeeID, emp.getGrade(),sal,grade);
        return res;
    }

    return SUCCESS;
}


StatusType EmployeeManager::AcquireCompany(int AcquirerID, int TargetID, double Factor)
{
    if(AcquirerID <= 0 || TargetID <= 0 || AcquirerID == TargetID || Factor < 1.00)
        return INVALID_INPUT;

    Node<int, int>* AcquirerExists = CompanyTree.find(AcquirerID);
    Node<int, int>* TargetExists = CompanyTree.find(TargetID);
    if(AcquirerExists == nullptr || TargetExists == nullptr)
        return FAILURE;

    int AcquirerValue = *AcquirerExists->getData();
    int TargetValue = *TargetExists->getData();
    if(AcquirerValue < 10 * TargetValue)
        return FAILURE;

    //now we find the companies in the hiringTree
    Node<int, HiringCompany>* AcquirerCompany = HiringCompaniesTree.find(AcquirerID);
    Node<int, HiringCompany>* TargetCompany = HiringCompaniesTree.find(TargetID);

    int AcqNewValue = floor((AcquirerValue + TargetValue) * Factor);

    //case I: target doesnt have employees - simply remove and change acquirer value
    if(TargetCompany == nullptr)
    {
        RemoveCompany(TargetID);
        *AcquirerExists->getData() = AcqNewValue;
        if(AcquirerCompany != nullptr)
            AcquirerCompany->getData()->setCompanyValue(AcqNewValue);
        return SUCCESS;
    }

    //case II: target has employees but acquirer doesnt
    else if(AcquirerCompany == nullptr)
    {
        int targetNumOfEmployees = TargetCompany->getData()->getCompanyNumOfEmployees();

        int* targetIDKeys = nullptr;
        SalaryTreeKey* targetSalaryKeys = nullptr;
        Employee* targetIDEmployees = nullptr;
        Employee* targetSalaryEmployees = nullptr;

        targetIDKeys = new int[targetNumOfEmployees];
        targetIDEmployees = new Employee[targetNumOfEmployees];
        targetSalaryKeys = new SalaryTreeKey[targetNumOfEmployees];
        targetSalaryEmployees = new Employee[targetNumOfEmployees];

        CompanyRes res = TargetCompany->getData()->fillArrayKeyAndDataIDTree(targetIDKeys, targetIDEmployees);
        if(res != COMPANY_SUCCESS)
        {
            delete[] targetIDKeys;
            delete[] targetIDEmployees;
            delete[] targetSalaryKeys;
            delete[] targetSalaryEmployees;
            return FAILURE;
        }

        res = TargetCompany->getData()->fillArrayKeyAndDataSalaryTree(targetSalaryKeys,targetSalaryEmployees);
        if(res != COMPANY_SUCCESS)
        {
            delete[] targetIDKeys;
            delete[] targetIDEmployees;
            delete[] targetSalaryKeys;
            delete[] targetSalaryEmployees;
            return FAILURE;
        }

        for(int i = 0; i < targetNumOfEmployees; i++)
        {
            targetIDEmployees[i].getEmployeeInIDTree()->getData()->changeCompany(AcquirerID);
            targetIDEmployees[i].getEmployeeInSalaryTree()->getData()->changeCompany(AcquirerID);
        }

        //creating the hiring company and adding to tree;
        HiringCompany acquirer = HiringCompany(AcquirerID, AcqNewValue);
        HiringCompaniesTree.insert(AcquirerID, acquirer);

        Node<int, HiringCompany>* acqInHiringTree = HiringCompaniesTree.find(AcquirerID);

        acqInHiringTree->getData()->setCompanyNumOfEmployees(targetNumOfEmployees);
        acqInHiringTree->getData()->setCompanyHighestEarner(TargetCompany->getData()->getCompanyHighestEarner());
        acqInHiringTree->getData()->CreateIDTree(targetIDKeys, targetIDEmployees, targetNumOfEmployees);
        acqInHiringTree->getData()->CreateSalaryTree(targetSalaryKeys, targetSalaryEmployees, targetNumOfEmployees);


        HiringCompaniesTree.remove(TargetID);
        CompanyTree.remove(TargetID);

        delete[] targetIDKeys;
        delete[] targetIDEmployees;
        delete[] targetSalaryKeys;
        delete[] targetSalaryEmployees;

        *AcquirerExists->getData() = AcqNewValue;
    }

    //case III: both have workers - merge sort
    else
    {
        int numTargetEmps = TargetCompany->getData()->getCompanyNumOfEmployees();
        int numAcquirerEmps = AcquirerCompany->getData()->getCompanyNumOfEmployees();

        int* targetIDKeys = new int[numTargetEmps];
        Employee* targetIDEmps = new Employee[numTargetEmps];

        int* acquirerIDKeys = new int[numAcquirerEmps];
        Employee* acquirerIDEmps = new Employee[numAcquirerEmps];

        int* mergedIDKeys = new int[numTargetEmps + numAcquirerEmps];
        Employee* mergedIDEmps = new Employee[numTargetEmps + numAcquirerEmps];

        TargetCompany->getData()->fillArrayKeyAndDataIDTree(targetIDKeys, targetIDEmps);
        AcquirerCompany->getData()->fillArrayKeyAndDataIDTree(acquirerIDKeys, acquirerIDEmps);


        //merging the id arrays
        int targetCount = 0, acquirerCount = 0;
        for(int i = 0; i < numTargetEmps + numAcquirerEmps; i++)
        {
            if((targetCount == numTargetEmps) || (acquirerCount != numAcquirerEmps && acquirerIDKeys[acquirerCount] < targetIDKeys[targetCount]))
            {
                mergedIDKeys[i] = acquirerIDKeys[acquirerCount];
                mergedIDEmps[i] = acquirerIDEmps[acquirerCount];
                acquirerCount++;

            }
            else if((acquirerCount == numAcquirerEmps) || (targetCount != numTargetEmps && targetIDKeys[targetCount] < acquirerIDKeys[acquirerCount]))
            {
                mergedIDKeys[i] = targetIDKeys[targetCount];
                mergedIDEmps[i] = targetIDEmps[targetCount];
                targetCount++;
            }
        }



        SalaryTreeKey* targetSalaryKeys = new SalaryTreeKey[numTargetEmps];
        Employee* targetSalaryEmps = new Employee[numTargetEmps];

        SalaryTreeKey* acquirerSalaryKeys = new SalaryTreeKey[numAcquirerEmps];
        Employee* acquirerSalaryEmps = new Employee[numAcquirerEmps];

        SalaryTreeKey* mergedSalaryKeys = new SalaryTreeKey[numTargetEmps + numAcquirerEmps];
        Employee* mergedSalaryEmps = new Employee[numTargetEmps + numAcquirerEmps];

        TargetCompany->getData()->fillArrayKeyAndDataSalaryTree(targetSalaryKeys, targetSalaryEmps);
        AcquirerCompany->getData()->fillArrayKeyAndDataSalaryTree(acquirerSalaryKeys, acquirerSalaryEmps);


        //merging salary arrays
        targetCount = 0;
        acquirerCount = 0;
        for(int i =0; i< numTargetEmps + numAcquirerEmps; i++)
        {
            if((targetCount == numTargetEmps) || (acquirerCount != numAcquirerEmps && acquirerSalaryKeys[acquirerCount] < targetSalaryKeys[targetCount]))
            {
                mergedSalaryKeys[i] = acquirerSalaryKeys[acquirerCount];
                mergedSalaryEmps[i] = acquirerSalaryEmps[acquirerCount];
                acquirerCount++;
            }
            else if((acquirerCount == numAcquirerEmps) || (targetCount != numTargetEmps && targetSalaryKeys[targetCount] < acquirerSalaryKeys[acquirerCount]))
            {
                mergedSalaryKeys[i] = targetSalaryKeys[targetCount];
                mergedSalaryEmps[i] = targetSalaryEmps[targetCount];
                targetCount++;
            }
        }

        //changing emps company id in general emp tree
        for(int i =0; i < numTargetEmps; i++)
        {
            targetIDEmps[i].getEmployeeInIDTree()->getData()->changeCompany(AcquirerID);
            targetIDEmps[i].getEmployeeInSalaryTree()->getData()->changeCompany(AcquirerID);
        }

        HighestEarner targetHighest = TargetCompany->getData()->getCompanyHighestEarner();
        HighestEarner acquirerHighest = AcquirerCompany->getData()->getCompanyHighestEarner();
        HighestEarner newHighest = acquirerHighest;
        newHighest.compareAndSetSalaries(targetHighest.getID(), targetHighest.getSalary());


        HiringCompaniesTree.remove(AcquirerID);
        HiringCompany newAcquirerComp(AcquirerID, AcqNewValue);
        HiringCompaniesTree.insert(AcquirerID, newAcquirerComp);
        AcquirerCompany = HiringCompaniesTree.find(AcquirerID);



        AcquirerCompany->getData()->CreateIDTree(mergedIDKeys, mergedIDEmps, numTargetEmps + numAcquirerEmps);
        AcquirerCompany->getData()->CreateSalaryTree(mergedSalaryKeys, mergedSalaryEmps, numTargetEmps + numAcquirerEmps);
        AcquirerCompany->getData()->setCompanyNumOfEmployees(numTargetEmps + numAcquirerEmps);
        AcquirerCompany->getData()->setCompanyHighestEarner(newHighest);

        *AcquirerExists->getData() = AcqNewValue;



        HiringCompaniesTree.remove(TargetID);
        numOfHiringCompanies--;
        CompanyTree.remove(TargetID);

        delete[] targetIDKeys;
        delete[] targetIDEmps;
        delete[] acquirerIDKeys;
        delete[] acquirerIDEmps;
        delete[] mergedIDEmps;
        delete[] mergedIDKeys;
        delete[] targetSalaryKeys;
        delete[] targetSalaryEmps;
        delete[] acquirerSalaryKeys;
        delete[] acquirerSalaryEmps;
        delete[] mergedSalaryKeys;
        delete[] mergedSalaryEmps;
    }

    return SUCCESS;
}

StatusType EmployeeManager::GetHighestEarner(int CompanyID, int *EmployeeID)
{
    if(CompanyID == 0 || EmployeeID == nullptr)
        return INVALID_INPUT;

    if(totalEmployees == 0)
        return FAILURE;

    if(CompanyID < 0)
        *EmployeeID = highestEarner.getID();
    if(CompanyID > 0)
    {
        Node<int, HiringCompany>* found = HiringCompaniesTree.find(CompanyID);
        if(found == nullptr)
            return FAILURE;
        *EmployeeID = found->getData()->getCompanyHighestEarner().getID();
    }
    return SUCCESS;
}

StatusType EmployeeManager::GetAllEmployeesBySalary(int CompanyID, int **Employees, int *NumOfEmployees)
{
    if(CompanyID == 0 || Employees == nullptr || NumOfEmployees == nullptr)
        return INVALID_INPUT;

    if(totalEmployees == 0)
        return FAILURE;

    if(CompanyID < 0)
    {
        *NumOfEmployees = totalEmployees;
        *Employees = (int*) malloc(sizeof(int) * totalEmployees);
        if(*Employees == nullptr)
            return ALLOCATION_ERROR;

        SalaryTreeKey* arr = new SalaryTreeKey[totalEmployees];
        TreeRes res = EmployeeSalaryTree.reverseInOrderFillKeyInArray(arr);
        if(res != TREE_SUCCESS)
        {
            delete[] arr;
            free(*Employees);
            return FAILURE;
        }
        for(int i = 0; i < totalEmployees; i++)
            (*Employees)[i] = arr[i].getID();
        delete[] arr;
    }

    if(CompanyID > 0)
    {
        Node<int, HiringCompany>* found = HiringCompaniesTree.find(CompanyID);
        if(found == nullptr)
            return FAILURE;
        *NumOfEmployees = found->getData()->getCompanyNumOfEmployees();
        *Employees = (int*) malloc(sizeof(int) * (*NumOfEmployees));
        if(*Employees == nullptr)
            return ALLOCATION_ERROR;
        SalaryTreeKey* arr = new SalaryTreeKey[*NumOfEmployees];
        CompanyRes r = found->getData()->fillArrayReverseSalaryTreeKey(arr);
        if(r != COMPANY_SUCCESS)
        {
            free(Employees);
            delete[] arr;
            return FAILURE;
        }
        for(int i = 0; i < *NumOfEmployees; i++)
            (*Employees)[i] = arr[i].getID();
        delete[] arr;
    }
    return SUCCESS;
}

StatusType EmployeeManager::GetHighestEarnerInEachCompany(int NumOfCompanies, int **Employees)
{
    if(Employees == nullptr || NumOfCompanies < 1)
        return INVALID_INPUT;
    if(numOfHiringCompanies < NumOfCompanies)
        return FAILURE;

    if(NumOfCompanies == 0)
        return FAILURE;


    *Employees = (int*) malloc(sizeof (int) * NumOfCompanies);
    if(*Employees == nullptr)
        return ALLOCATION_ERROR;

    TreeRes res = HiringCompaniesTree.limitedIterationsInOrder(*Employees, NumOfCompanies);
    if(res != TREE_SUCCESS)
    {
        return FAILURE;
    }


    return SUCCESS;
}

StatusType EmployeeManager::GetNumEmployeesMatching(int CompanyID, int MinEmployeeID, int MaxEmployeeID, int MinSalary,
                                                    int MinGrade, int *TotalNumOfEmployees, int *NumOfEmployees)
{
    if(TotalNumOfEmployees == nullptr || NumOfEmployees == nullptr || CompanyID == 0 ||
       MinEmployeeID < 0 || MaxEmployeeID < 0 || MinSalary < 0 || MinGrade < 0 || MaxEmployeeID < MinEmployeeID)
        return INVALID_INPUT;

    if(CompanyID > 0)
    {
        Node<int, HiringCompany>* found = HiringCompaniesTree.find(CompanyID);
        if(found == nullptr)
            return FAILURE;
        CompanyRes res = found->getData()->GetNumMatchingInCompany(MinEmployeeID, MaxEmployeeID, MinSalary,
                                                                   MinGrade, TotalNumOfEmployees, NumOfEmployees);
        if(res != COMPANY_SUCCESS)
            return FAILURE;
        return SUCCESS;
    }
    if(CompanyID < 0)
    {
        if(totalEmployees == 0)
            return FAILURE;

        *TotalNumOfEmployees = EmployeeIDTree.findAmountInInterval(MinEmployeeID, MaxEmployeeID);

        Employee* arr = new Employee[*TotalNumOfEmployees];
        TreeRes res = EmployeeIDTree.fillMatchingInArr(arr, MinEmployeeID, MaxEmployeeID);
        if(res != TREE_SUCCESS)
        {
            delete[] arr;
            return FAILURE;
        }

        int counter = 0;
        for(int i = 0; i < *TotalNumOfEmployees; i++)
        {
            if(arr[i].getGrade() >= MinGrade && arr[i].getSalary() >= MinSalary)
                counter++;
        }
        *NumOfEmployees = counter;
        delete[] arr;
    }
    return SUCCESS;
}
