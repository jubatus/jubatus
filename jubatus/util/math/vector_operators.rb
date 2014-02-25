
class Array
  def jk() # jk is join by comma, JK...
    self.join(',')
  end
  def jr() # JR, new line.
    self.join("\n")
  end
  def mjk() # map and join by comma? mjk...
    self.map{|x| yield(x)}.jk()
  end
  def mjsk() # you abbreviate that?! mjsk
    self.map{|x| yield(x)}.join(';')
  end
end



def vector_operators(template_dec, scalar, mytype, constructor, dim, cs)
  ret=''
  ret+= <<CPPEND
  ///vector addition
  #{template_dec}
    #{mytype} operator+(const #{mytype} & a, const #{mytype} & b){
    return #{constructor[cs.map{|x| "a.#{x} + b.#{x}"}]};
  }

  ///vector subtraction
  #{template_dec}
    #{mytype} operator-(const #{mytype} & a, const #{mytype} & b){
    return #{constructor[cs.map{|x| "a.#{x} - b.#{x}"}]};
  }

  ///vector negate
  #{template_dec}
    #{mytype} operator-(const #{mytype} & a){
    return #{constructor[cs.map{|x| "-a.#{x}"}]};
  }

  ///scalar multiplication
  #{template_dec}
    #{mytype} operator*(const #{scalar} &m, const #{mytype} & a){
    return #{constructor[cs.map{|x| "m*a.#{x}"}]};
  }
  #{template_dec}
    #{mytype} operator*(const #{mytype} & a, const #{scalar} &m){
    return #{constructor[cs.map{|x| "m*a.#{x}"}]};
  }

  ///scalar multiplication by reciprocal of \\a m
  #{template_dec}
    #{mytype} operator/(const #{mytype} & a, const #{scalar} &m){
    return #{constructor[cs.map{|x| "a.#{x}/m"}]};
  }

  ///inner product
  #{template_dec}
    #{scalar} operator*(const #{mytype} &a, const #{mytype} & b){
    return #{cs.map{|x| "a.#{x}*b.#{x}"}.join(' + ')};
  }

CPPEND

  ret+= <<CPPEND if dim == 2
  ///outer product; outer product uses % operator.
  #{template_dec}
    #{scalar} operator%(const #{mytype} & a, const #{mytype} & b){
    return a.x*b.y - a.y*b.x;
  }
CPPEND
  ret+= <<CPPEND if dim == 3 
  ///outer product; outer product uses % operator.
  #{template_dec}
    #{mytype} operator%(const #{mytype} & a, const #{mytype} & b){
    return #{constructor['a.y*b.z - a.z*b.y', 'a.z*b.x - a.x*b.z', 'a.x*b.y - a.y*b.x' ]};
  }
CPPEND

  ret+= <<CPPEND

  /// +=
  #{template_dec}
    #{mytype}& operator+=(#{mytype} &a, const #{mytype} &b){
    return a=a+b;
  }
  /// -=
  #{template_dec}
    #{mytype}& operator-=(#{mytype} &a, const #{mytype} &b){
    return a=a-b;
  }
  /// *=
  #{template_dec}
    #{mytype}& operator*=(#{mytype} &a, const #{scalar} &b){
    return a=b*a;
  }
  /// /=
  #{template_dec}
    #{mytype}& operator/=(#{mytype} &a, const #{scalar} &b){
    return a=a/b;
  }



  ///equality between two vectors
  #{template_dec}
    bool operator==(const #{mytype} & a, const #{mytype} & b){
    return #{cs.map{|x| "a.#{x}==b.#{x}"}.join(' && ')};
  }

  ///dictionary order of vectors
  #{template_dec}
    bool operator<(const #{mytype} & a, const #{mytype} & b){
#{cs[0..-2].map{|x| "    if(a.#{x} != b.#{x}) return a.#{x} < b.#{x};"}.jr}
    return a.#{cs[-1]} < b.#{cs[-1]};
  }

  ///square of absolute value
  #{template_dec}
    #{scalar} norm(const #{mytype} & a){
    return a*a;
  }

  ///absolute value
  #{template_dec} 
    #{scalar} abs(const #{mytype} & a){
    return std::sqrt(norm(a));
  }

  ///normalization (return new vector, of length \\a a and parallel to \\a *this)
  ///default value of \\a a is 1
  #{template_dec} 
    #{mytype} normalize(const #{mytype} &a, #{scalar} b=1){
    return (b/std::abs(a)) * (a);
  }

  ///projection factor of vector \\a a in direction of \\a b
  #{template_dec}
    #{scalar} operator/(const #{mytype} & a, const #{mytype} & b){
    return (a*b) / (b*b);
  }

CPPEND

end
