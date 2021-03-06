/**************************************************************************/ /**
 *
 * \file
 * \brief Implementation of an abstract domain keeping track of null variables
 *
 * Author: Maxime Arthaud
 *
 * Contact: ikos@lists.nasa.gov
 *
 * Notices:
 *
 * Copyright (c) 2018 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Disclaimers:
 *
 * No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF
 * ANY KIND, EITHER EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL CONFORM TO SPECIFICATIONS,
 * ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * OR FREEDOM FROM INFRINGEMENT, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL BE
 * ERROR FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED, WILL CONFORM TO
 * THE SUBJECT SOFTWARE. THIS AGREEMENT DOES NOT, IN ANY MANNER, CONSTITUTE AN
 * ENDORSEMENT BY GOVERNMENT AGENCY OR ANY PRIOR RECIPIENT OF ANY RESULTS,
 * RESULTING DESIGNS, HARDWARE, SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS
 * RESULTING FROM USE OF THE SUBJECT SOFTWARE.  FURTHER, GOVERNMENT AGENCY
 * DISCLAIMS ALL WARRANTIES AND LIABILITIES REGARDING THIRD-PARTY SOFTWARE,
 * IF PRESENT IN THE ORIGINAL SOFTWARE, AND DISTRIBUTES IT "AS IS."
 *
 * Waiver and Indemnity:  RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST
 * THE UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL
 * AS ANY PRIOR RECIPIENT.  IF RECIPIENT'S USE OF THE SUBJECT SOFTWARE RESULTS
 * IN ANY LIABILITIES, DEMANDS, DAMAGES, EXPENSES OR LOSSES ARISING FROM SUCH
 * USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM,
 * RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD
 * HARMLESS THE UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS,
 * AS WELL AS ANY PRIOR RECIPIENT, TO THE EXTENT PERMITTED BY LAW.
 * RECIPIENT'S SOLE REMEDY FOR ANY SUCH MATTER SHALL BE THE IMMEDIATE,
 * UNILATERAL TERMINATION OF THIS AGREEMENT.
 *
 ******************************************************************************/

#pragma once

#include <ikos/core/domain/nullity/abstract_domain.hpp>
#include <ikos/core/domain/separate_domain.hpp>

namespace ikos {
namespace core {
namespace nullity {

/// \brief Nullity abstract domain
///
/// Implementation of the nullity abstract domain interface using SeparateDomain
template < typename VariableRef >
class NullityDomain final
    : public nullity::AbstractDomain< VariableRef,
                                      NullityDomain< VariableRef > > {
private:
  using SeparateDomainT = SeparateDomain< VariableRef, Nullity >;

public:
  using Iterator = typename SeparateDomainT::Iterator;

private:
  SeparateDomainT _inv;

private:
  /// \brief Private constructor
  explicit NullityDomain(SeparateDomainT inv) : _inv(std::move(inv)) {}

public:
  /// \brief Create the top abstract value
  NullityDomain() : _inv(SeparateDomainT::top()) {}

  /// \brief Copy constructor
  NullityDomain(const NullityDomain&) = default;

  /// \brief Move constructor
  NullityDomain(NullityDomain&&) = default;

  /// \brief Copy assignment operator
  NullityDomain& operator=(const NullityDomain&) = default;

  /// \brief Move assignment operator
  NullityDomain& operator=(NullityDomain&&) = default;

  /// \brief Destructor
  ~NullityDomain() override = default;

  /// \brief Create the top abstract value
  static NullityDomain top() { return NullityDomain(SeparateDomainT::top()); }

  /// \brief Create the bottom abstract value
  static NullityDomain bottom() {
    return NullityDomain(SeparateDomainT::bottom());
  }

  /// \brief Begin iterator over the pairs (variable, nullity)
  Iterator begin() const { return this->_inv.begin(); }

  /// \brief End iterator over the pairs (variable, nullity)
  Iterator end() const { return this->_inv.end(); }

  bool is_bottom() const override { return this->_inv.is_bottom(); }

  bool is_top() const override { return this->_inv.is_top(); }

  void set_to_bottom() override { this->_inv.set_to_bottom(); }

  void set_to_top() override { this->_inv.set_to_top(); }

  bool leq(const NullityDomain& other) const override {
    return this->_inv.leq(other._inv);
  }

  bool equals(const NullityDomain& other) const override {
    return this->_inv.equals(other._inv);
  }

  void join_with(const NullityDomain& other) override {
    this->_inv.join_with(other._inv);
  }

  void widen_with(const NullityDomain& other) override {
    this->_inv.widen_with(other._inv);
  }

  void meet_with(const NullityDomain& other) override {
    this->_inv.meet_with(other._inv);
  }

  void narrow_with(const NullityDomain& other) override {
    this->_inv.narrow_with(other._inv);
  }

  void assign_null(VariableRef x) override {
    this->_inv.set(x, Nullity::null());
  }

  void assign_non_null(VariableRef x) override {
    this->_inv.set(x, Nullity::non_null());
  }

  void assign(VariableRef x, VariableRef y) override {
    this->_inv.set(x, this->_inv.get(y));
  }

  void assert_null(VariableRef x) override {
    this->_inv.refine(x, Nullity::null());
  }

  void assert_non_null(VariableRef x) override {
    this->_inv.refine(x, Nullity::non_null());
  }

  void add(Predicate pred, VariableRef x, VariableRef y) override {
    if (this->is_bottom()) {
      return;
    }

    Nullity xn = this->_inv.get(x);
    Nullity yn = this->_inv.get(y);

    switch (pred) {
      case Predicate::EQ: {
        // x == y
        Nullity z = xn.meet(yn);
        this->_inv.set(x, z);
        this->_inv.set(y, z);
      } break;
      case Predicate::NE: {
        // x != y
        if (xn.is_null() && yn.is_null()) {
          this->_inv.set_to_bottom();
        } else if (xn.is_top() && yn.is_null()) {
          this->_inv.set(x, Nullity::non_null());
        } else if (xn.is_null() && yn.is_top()) {
          this->_inv.set(y, Nullity::non_null());
        }
      } break;
      case Predicate::GT: {
        this->add(Predicate::NE, x, y);
      } break;
      case Predicate::GE: {
        // nothing we can do.
      } break;
      case Predicate::LT: {
        this->add(Predicate::NE, x, y);
      } break;
      case Predicate::LE: {
        // nothing we can do.
      } break;
    }
  }

  bool is_null(VariableRef x) const override {
    ikos_assert_msg(!this->is_bottom(), "trying to call is_null() on bottom");
    return this->_inv.get(x).is_null();
  }

  bool is_non_null(VariableRef x) const override {
    ikos_assert_msg(!this->is_bottom(),
                    "trying to call is_non_null() on bottom");
    return this->_inv.get(x).is_non_null();
  }

  void set(VariableRef x, const Nullity& value) override {
    this->_inv.set(x, value);
  }

  void refine(VariableRef x, const Nullity& value) override {
    this->_inv.refine(x, value);
  }

  void forget(VariableRef x) override { this->_inv.forget(x); }

  void normalize() const override {}

  Nullity get(VariableRef x) const override { return this->_inv.get(x); }

  void dump(std::ostream& o) const override { return this->_inv.dump(o); }

  static std::string name() { return "nullity domain"; }

}; // end class NullityDomain

} // end namespace nullity
} // end namespace core
} // end namespace ikos
