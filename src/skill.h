#pragma once
#ifndef SKILL_H
#define SKILL_H

#include "calendar.h"
#include "string_id.h"

#include <functional>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <iosfwd>

class JsonObject;
class JsonIn;
class JsonOut;
class Skill;
using skill_id = string_id<Skill>;

class Skill
{
        skill_id _ident;

        std::string _name;
        std::string _description;
        std::set<std::string> _tags;
        // these are not real skills, they depend on context
        static std::map<skill_id, Skill> contextual_skills;
    public:
        static std::vector<Skill> skills;
        static void load_skill( JsonObject &jsobj );
        // For loading old saves that still have integer-based ids.
        static skill_id from_legacy_int( int legacy_id );
        static skill_id random_skill();

        static const Skill &get( const skill_id &id );

        static size_t skill_count();
        // clear skill vector, every skill pointer becomes invalid!
        static void reset();

        static std::vector<Skill const *> get_skills_sorted_by(
            std::function<bool ( Skill const &, Skill const & )> pred );

        Skill();
        Skill( skill_id ident, std::string name, std::string description,
               std::set<std::string> tags );

        skill_id const &ident() const {
            return _ident;
        }
        std::string const &name() const {
            return _name;
        }
        std::string const &description() const {
            return _description;
        }

        bool operator==( const Skill &b ) const {
            return this->_ident == b._ident;
        }
        bool operator< ( const Skill &b ) const {
            return this->_ident <  b._ident;    // Only here for the benefit of std::map<Skill,T>
        }

        bool operator!=( const Skill &b ) const {
            return !( *this == b );
        }

        bool is_combat_skill() const;
        bool is_contextual_skill() const;
};

class SkillLevel
{
        int _level;
        int _exercise;
        time_point _lastPracticed;
        bool _isTraining;
        int _highestLevel;

    public:
        SkillLevel( int level = 0, int exercise = 0, bool isTraining = true,
                    const time_point &lastPracticed = calendar::time_of_cataclysm, int highestLevel = 0 );
        SkillLevel( int minLevel, int maxLevel, int minExercise, int maxExercise, bool isTraining,
                    const time_point &lastPracticed, int highestLevel );

        bool isTraining() const {
            return _isTraining;
        }
        bool toggleTraining() {
            _isTraining = !_isTraining;
            return _isTraining;
        }

        int level() const {
            return _level;
        }
        int level( int plevel ) {
            _level = plevel;
            if( _level > _highestLevel ) {
                _highestLevel = _level;
            }
            return plevel;
        }

        int highestLevel() const {
            return _highestLevel;
        }

        int exercise( bool raw = false ) const {
            return raw ? _exercise : _exercise / ( ( _level + 1 ) * ( _level + 1 ) );
        }

        int exercised_level() const {
            return level() * level() * 100 + exercise();
        }

        const time_point &lastPracticed() const {
            return _lastPracticed;
        }

        void train( int amount, bool skip_scaling = false );
        bool isRusting() const;
        bool rust( bool charged_bio_mem );
        void practice();
        bool can_train() const;

        void readBook( int minimumGain, int maximumGain, int maximumLevel = -1 );

        bool operator==( const SkillLevel &b ) const {
            return this->_level == b._level && this->_exercise == b._exercise;
        }
        bool operator< ( const SkillLevel &b ) const {
            return this->_level <  b._level || ( this->_level == b._level && this->_exercise < b._exercise );
        }
        bool operator> ( const SkillLevel &b ) const {
            return this->_level >  b._level || ( this->_level == b._level && this->_exercise > b._exercise );
        }

        bool operator==( const int &b ) const {
            return this->_level == b;
        }
        bool operator< ( const int &b ) const {
            return this->_level <  b;
        }
        bool operator> ( const int &b ) const {
            return this->_level >  b;
        }

        bool operator!=( const SkillLevel &b ) const {
            return !( *this == b );
        }
        bool operator<=( const SkillLevel &b ) const {
            return !( *this >  b );
        }
        bool operator>=( const SkillLevel &b ) const {
            return !( *this <  b );
        }

        bool operator!=( const int &b ) const {
            return !( *this == b );
        }
        bool operator<=( const int &b ) const {
            return !( *this >  b );
        }
        bool operator>=( const int &b ) const {
            return !( *this <  b );
        }

        SkillLevel &operator= ( const SkillLevel & ) = default;

        void serialize( JsonOut &jsout ) const;
        void deserialize( JsonIn &jsin );

        // Make skillLevel act like a raw level by default.
        operator int() const {
            return _level;
        }
};

double price_adjustment( int );

#endif
