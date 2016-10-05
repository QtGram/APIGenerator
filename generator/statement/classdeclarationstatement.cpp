#include "classdeclarationstatement.h"
#include "signalsdeclarationstatements.h"

ClassDeclarationStatement::ClassDeclarationStatement(const QString &name, const QString& destinationdir): AbstractStatement(name, destinationdir)
{
    this->_baseclass = "QObject";
    this->_lastmethodvisibility = ClassDeclarationStatement::NO_VISIBILITY;
    this->_lastfieldvisibility = ClassDeclarationStatement::NO_VISIBILITY;
    this->_lastenumvisibility = ClassDeclarationStatement::NO_VISIBILITY;
}

ClassDeclarationStatement::~ClassDeclarationStatement()
{
    qDeleteAll(this->_methodlist);
    qDeleteAll(this->_fieldlist);
}

const QString &ClassDeclarationStatement::baseClass()
{
    return this->_baseclass;
}

void ClassDeclarationStatement::setBaseClass(const QString &baseclass)
{
    this->_baseclass = baseclass;
}

void ClassDeclarationStatement::includes(std::function<void(QSet<QString> &)> includeproc)
{
    includeproc(this->_includeset);
}

void ClassDeclarationStatement::properties(std::function<void(QList<PropertyDeclarationStatement *> &)> propertiesproc)
{
    QList<PropertyDeclarationStatement*> props;
    propertiesproc(props);

    foreach(PropertyDeclarationStatement* property, props)
    {
        this->_properties += property->toString();

        this->methods([this, property](QList<MethodDeclarationStatement*>& methods) {
            MethodDeclarationStatement* mds = NULL;
            MethodDeclarationStatement::Attributes typeattributes = (!property->isBasicType() || property->isVector()) ?
                                                                    MethodDeclarationStatement::CONST_BY_REFERENCE :
                                                                    MethodDeclarationStatement::NO_ATTRIBUTES;

            // Getter
            mds = new MethodDeclarationStatement(property->type(), this->camelCase(property->name()), AbstractStatement::CONST | typeattributes);
            this->_propertymap[mds->name()] = ClassDeclarationStatement::GETTER;
            this->_fieldmap[mds->name()] = property->name().toLower();
            methods << mds;

            // Setter
            mds = new MethodDeclarationStatement("void", QString("set%1").arg(this->camelCase(property->name(), true)));
            mds->addArgument(property->type(), property->name().toLower(), typeattributes);
            this->_propertymap[mds->name()] = ClassDeclarationStatement::SETTER;
            this->_fieldmap[mds->name()] = property->name().toLower();
            methods << mds;
        });

        this->emitters([property](QList<QString>& emits) {
            emits << property->name();
        });

        this->fields([property](QList<FieldDeclarationStatement*>& fieldlist) {
            fieldlist << new FieldDeclarationStatement(property->type(), "_" + property->name().toLower());
        });
    }

    qDeleteAll(props);
}

void ClassDeclarationStatement::enums(std::function<void (QList<EnumDeclarationStatement *> &)> enumproc)
{
    QList<EnumDeclarationStatement*> enums;
    enumproc(enums);

    foreach(EnumDeclarationStatement* enm, enums)
    {
        if(this->_lastenumvisibility != enm->visibility())
        {
            this->_enums += this->visibilityString(enm->visibility());
            this->_lastenumvisibility = enm->visibility();
        }

        this->_enums += enm->toString();
    }

    qDeleteAll(enums);
}

void ClassDeclarationStatement::emitters(std::function<void(QList<QString> &)> emitsproc)
{
    QList<QString> emits;
    emitsproc(emits);

    if(this->_signals.isEmpty())
        this->_signals += "\tsignals:\n";

    foreach(const QString& emitter, emits)
        this->_signals += SignalsDeclarationStatements(emitter).toString();
}

void ClassDeclarationStatement::write() const
{
    this->doWrite(".h");
}

QString ClassDeclarationStatement::toString() const
{
    QString content;

    foreach(const QString& include, this->_includeset)
    {
        if(include.startsWith("#include"))
            content += include + "\n";
        else if(include.startsWith("<") && include.endsWith(">"))
            content += "#include " + include + "\n";
        else
            content += "#include \"" + include.toLower() + "\"\n";
    }

    content += "\n";
    content += "class " + this->_name + ": public " + this->_baseclass + "\n";
    content += "{\n";
    content += "\tQ_OBJECT\n\n";

    if(!this->_properties.isEmpty())
        content += this->_properties + "\n";

    if(!this->_enums.isEmpty())
        content += this->_enums + "\n";

    if(!this->_methods.isEmpty())
        content += this->_methods + "\n";

    if(!this->_signals.isEmpty())
        content += this->_signals + "\n";

    if(!this->_fields.isEmpty())
        content += this->_fields + "\n";

    content += "};\n";
    return content;
}

void ClassDeclarationStatement::methods(std::function<void(QList<MethodDeclarationStatement *>&)> methodsproc)
{
    QList<MethodDeclarationStatement*> methods;
    methodsproc(methods);

    foreach(MethodDeclarationStatement* method, methods)
    {
        if(this->_lastmethodvisibility != method->visibility())
        {
            this->_methods += this->visibilityString(method->visibility());
            this->_lastmethodvisibility = method->visibility();
        }

        this->_methods += method->toString();
    }

    this->_methodlist.append(methods);
}

void ClassDeclarationStatement::fields(std::function<void(QList<FieldDeclarationStatement *>&)> fieldsproc)
{
    QList<FieldDeclarationStatement*> fields;
    fieldsproc(fields);

    foreach(FieldDeclarationStatement* field, fields)
    {
        if(this->_lastfieldvisibility != field->visibility())
        {
            this->_fields = this->visibilityString(field->visibility());
            this->_lastfieldvisibility = field->visibility();
        }

        this->_fields += field->toString();
    }

    this->_fieldlist.append(fields);
}
