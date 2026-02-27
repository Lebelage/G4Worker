#pragma once
#include "G4UIcommand.hh"
#include "string.h"
#include <memory>
#include <vector>

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"

struct Param
{
    std::string name;
    char type;
    bool omittable = false;

    Param(std::string n, char t, bool om = false)
        : name(std::move(n)), type(t), omittable(om) {}
};

namespace G4Worker::Messengers
{
    class G4UICommandBuilder
    {
    public:
        explicit G4UICommandBuilder(G4UImessenger *owner)
            : owner(owner)
        {
        }

        // Директория
        G4UIdirectory *Directory(std::string_view path, std::string_view guidance)
        {
            auto *dir = new G4UIdirectory(std::string(path).c_str());
            dir->SetGuidance(std::string(guidance).c_str());
            return dir;
        }

        // Простая команда без параметров
        G4UIcommand *Command(std::string_view path)
        {
            return new G4UIcommand(std::string(path).c_str(), owner);
        }

        // Строковая команда
        G4UIcmdWithAString *String(std::string_view path)
        {
            return new G4UIcmdWithAString(std::string(path).c_str(), owner);
        }

        // Число + единицы
        G4UIcmdWithADoubleAndUnit *DoubleUnit(std::string_view path, std::string_view unitCategory)
        {
            auto *cmd = new G4UIcmdWithADoubleAndUnit(std::string(path).c_str(), owner);
            cmd->SetUnitCategory(std::string(unitCategory).c_str());
            return cmd;
        }

        // Вектор + единицы
        G4UIcmdWith3VectorAndUnit *Vec3Unit(std::string_view path, std::string_view unitCategory)
        {
            auto *cmd = new G4UIcmdWith3VectorAndUnit(std::string(path).c_str(), owner);
            cmd->SetUnitCategory(std::string(unitCategory).c_str());
            return cmd;
        }

        // Команда со списком параметров
        template <typename... Args>
        G4UIcommand *Params(std::string_view path, Args &&...params)
        {
            auto *cmd = new G4UIcommand(std::string(path).c_str(), owner);

            (AddParam(cmd, std::forward<Args>(params)), ...);

            return cmd;
        }

        // Установка кандидатов для строковой команды
        G4UIcmdWithAString *Candidates(std::string_view path, std::string_view options)
        {
            auto *cmd = new G4UIcmdWithAString(std::string(path).c_str(), owner);
            cmd->SetCandidates(std::string(options).c_str());
            return cmd;
        }

    private:
        G4UImessenger *owner;

        // Добавление параметра
        void AddParam(G4UIcommand *cmd, const Param &p)
        {
            auto *prm = new G4UIparameter(p.name.c_str(), p.type, p.omittable);
            cmd->SetParameter(prm);
        }
    };
}