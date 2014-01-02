% Produce a flow matching the given boundary layer edge conditions.
% If supplied, T_e specifies an edge temperature for the ideal gas EOS.
function [R0 Ma0 u1 rho1 p1] = nozzle_match(delta, gam0, Ma_e, p_exi, T_e=0)
  R0   = max(roots([
          - abs(Ma_e.^2 - 1)*abs(p_exi),
            delta,
          - Ma_e.^2 * delta.^2 * abs(p_exi) * sign(Ma_e.^2 - 1)
         ]));
  Ma0  = 1 / realsqrt(1/Ma_e.^2 + (gam0 - 1)*delta.^2/R0.^2/2);
  R    = realsqrt(R0.^2 + delta.^2);
  u1   = - R / R0 * sign(p_exi*(Ma_e.^2 - 1));
  rho1 = 1;
  p1   = merge(T_e == 0, 1, T_e*Ma0.^2*rho1/gam0/Ma_e.^2);
  if (iscomplex(R0))
    warning('nozzle_match(%g, %g, %g, %g, %g) has complex root: %g', ...
            delta, gam0, Ma_e, p_exi, T_e, R0);
  endif
end
