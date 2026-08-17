import numpy as np
import pandas as pd
import streamlit as st
import plotly.graph_objects as go
from math import exp, log, sqrt, erf

st.set_page_config(page_title="Monte Carlo Option Pricer", page_icon="🎲", layout="wide")


def norm_cdf(x: float) -> float:
    return 0.5 * (1.0 + erf(x / sqrt(2.0)))


def black_scholes_price(S, K, T, r, sigma, option_type):
    if T <= 0:
        return max(S - K, 0.0) if option_type == "Call" else max(K - S, 0.0)
    if sigma <= 0:
        forward = S * exp(r * T)
        payoff = max(forward - K, 0.0) if option_type == "Call" else max(K - forward, 0.0)
        return exp(-r * T) * payoff
    d1 = (log(S / K) + (r + 0.5 * sigma**2) * T) / (sigma * sqrt(T))
    d2 = d1 - sigma * sqrt(T)
    if option_type == "Call":
        return S * norm_cdf(d1) - K * exp(-r * T) * norm_cdf(d2)
    return K * exp(-r * T) * norm_cdf(-d2) - S * norm_cdf(-d1)


def bs_greeks(S, K, T, r, sigma, option_type):
    if T <= 0 or sigma <= 0:
        return {"Delta": np.nan, "Gamma": np.nan, "Vega": np.nan, "Theta": np.nan, "Rho": np.nan}
    d1 = (log(S / K) + (r + 0.5 * sigma**2) * T) / (sigma * sqrt(T))
    d2 = d1 - sigma * sqrt(T)
    pdf = exp(-0.5 * d1**2) / sqrt(2 * np.pi)
    delta = norm_cdf(d1) if option_type == "Call" else norm_cdf(d1) - 1.0
    gamma = pdf / (S * sigma * sqrt(T))
    vega = S * pdf * sqrt(T) / 100.0
    if option_type == "Call":
        theta = (-(S * pdf * sigma) / (2 * sqrt(T)) - r * K * exp(-r * T) * norm_cdf(d2)) / 365.0
        rho = K * T * exp(-r * T) * norm_cdf(d2) / 100.0
    else:
        theta = (-(S * pdf * sigma) / (2 * sqrt(T)) + r * K * exp(-r * T) * norm_cdf(-d2)) / 365.0
        rho = -K * T * exp(-r * T) * norm_cdf(-d2) / 100.0
    return {"Delta": delta, "Gamma": gamma, "Vega": vega, "Theta": theta, "Rho": rho}


def simulate_gbm(S, T, r, sigma, simulations, steps, seed, antithetic=False):
    rng = np.random.default_rng(seed)
    half = (simulations + 1) // 2 if antithetic else simulations
    dt = T / steps
    drift = (r - 0.5 * sigma**2) * dt
    vol = sigma * sqrt(dt)
    z = rng.standard_normal((half, steps))
    if antithetic:
        z = np.vstack([z, -z])[:simulations]
    else:
        z = z[:simulations]
    log_paths = np.cumsum(drift + vol * z, axis=1)
    terminal = S * np.exp(log_paths[:, -1])
    return terminal, z, log_paths


def monte_carlo_european(S, K, T, r, sigma, simulations, steps, option_type, seed, antithetic=False, control_variate=False):
    terminal, _, _ = simulate_gbm(S, T, r, sigma, simulations, steps, seed, antithetic)
    discounted = exp(-r * T)
    if option_type == "Call":
        payoff = np.maximum(terminal - K, 0.0)
    else:
        payoff = np.maximum(K - terminal, 0.0)
    raw = discounted * payoff
    control_adjusted = raw.copy()
    if control_variate:
        control = discounted * terminal
        expected_control = S
        cov = np.cov(raw, control, ddof=1)[0, 1]
        var = np.var(control, ddof=1)
        beta = cov / var if var > 0 else 0.0
        control_adjusted = raw - beta * (control - expected_control)
    price = float(np.mean(control_adjusted))
    stderr = float(np.std(control_adjusted, ddof=1) / sqrt(len(control_adjusted)))
    ci_low = price - 1.96 * stderr
    ci_high = price + 1.96 * stderr
    return price, stderr, ci_low, ci_high, terminal, raw, control_adjusted


def monte_carlo_asian(S, K, T, r, sigma, simulations, steps, option_type, seed, antithetic=False):
    _, z, log_paths = simulate_gbm(S, T, r, sigma, simulations, steps, seed, antithetic)
    paths = S * np.exp(log_paths)
    average = np.mean(paths, axis=1)
    if option_type == "Call":
        payoff = np.maximum(average - K, 0.0)
    else:
        payoff = np.maximum(K - average, 0.0)
    discounted = exp(-r * T) * payoff
    price = float(np.mean(discounted))
    stderr = float(np.std(discounted, ddof=1) / sqrt(len(discounted)))
    return price, stderr, price - 1.96 * stderr, price + 1.96 * stderr


def convergence_data(S, K, T, r, sigma, option_type, max_simulations, steps, seed, antithetic=False):
    counts = np.unique(np.geomspace(1000, max_simulations, 12).astype(int))
    rng = np.random.default_rng(seed)
    prices = []
    bs = black_scholes_price(S, K, T, r, sigma, option_type)
    for n in counts:
        sims = rng.integers(0, 2**32 - 1)
        price, *_ = monte_carlo_european(S, K, T, r, sigma, int(n), steps, option_type, int(sims), antithetic, False)
        prices.append(price)
    return counts, np.array(prices), bs


st.title("🎲 Monte Carlo Option Pricer")
st.caption("C++ pricing-engine concepts rebuilt as an interactive Streamlit research dashboard")

with st.sidebar:
    st.header("Market Inputs")
    S = st.number_input("Spot Price (S)", min_value=0.01, value=100.0, step=1.0)
    K = st.number_input("Strike Price (K)", min_value=0.01, value=100.0, step=1.0)
    T = st.number_input("Time to Maturity (Years)", min_value=0.01, value=1.0, step=0.25)
    r = st.number_input("Risk-free Rate", min_value=-0.10, max_value=1.0, value=0.05, step=0.005, format="%.3f")
    sigma = st.number_input("Volatility", min_value=0.001, max_value=3.0, value=0.20, step=0.01, format="%.3f")
    option_type = st.selectbox("Option Type", ["Call", "Put"])
    st.header("Simulation")
    simulations = st.slider("Simulations", 1_000, 500_000, 100_000, step=1_000)
    steps = st.slider("Time Steps", 1, 250, 50)
    seed = st.number_input("Random Seed", min_value=0, value=42, step=1)
    antithetic = st.checkbox("Antithetic Variates", value=True)
    control_variate = st.checkbox("Control Variate", value=True)

mc_price, stderr, ci_low, ci_high, terminal, raw, adjusted = monte_carlo_european(
    S, K, T, r, sigma, simulations, steps, option_type, int(seed), antithetic, control_variate
)
bs_price = black_scholes_price(S, K, T, r, sigma, option_type)
greeks = bs_greeks(S, K, T, r, sigma, option_type)

col1, col2, col3, col4 = st.columns(4)
col1.metric("Monte Carlo Price", f"{mc_price:.4f}")
col2.metric("Black-Scholes", f"{bs_price:.4f}")
col3.metric("Pricing Error", f"{mc_price - bs_price:.4f}")
col4.metric("95% CI", f"[{ci_low:.4f}, {ci_high:.4f}]")

st.subheader("Pricing Comparison")
comparison = pd.DataFrame({"Method": ["Monte Carlo", "Black-Scholes"], "Price": [mc_price, bs_price]})
st.bar_chart(comparison.set_index("Method"))

left, right = st.columns(2)
with left:
    st.subheader("Simulation Distribution")
    fig = go.Figure()
    fig.add_trace(go.Histogram(x=terminal, nbinsx=60, name="Terminal Price"))
    fig.add_vline(x=K, line_dash="dash", annotation_text="Strike")
    fig.update_layout(xaxis_title="Terminal Asset Price", yaxis_title="Frequency", height=420)
    st.plotly_chart(fig, use_container_width=True)
with right:
    st.subheader("Payoff Distribution")
    fig2 = go.Figure()
    fig2.add_trace(go.Histogram(x=adjusted, nbinsx=60, name="Discounted Payoff"))
    fig2.update_layout(xaxis_title="Discounted Payoff", yaxis_title="Frequency", height=420)
    st.plotly_chart(fig2, use_container_width=True)

st.subheader("Black-Scholes Greeks")
gcols = st.columns(5)
for c, (name, value) in zip(gcols, greeks.items()):
    c.metric(name, "N/A" if np.isnan(value) else f"{value:.6f}")

st.subheader("Monte Carlo Convergence")
counts, prices, benchmark = convergence_data(S, K, T, r, sigma, option_type, simulations, steps, int(seed), antithetic)
conv_fig = go.Figure()
conv_fig.add_trace(go.Scatter(x=counts, y=prices, mode="lines+markers", name="Monte Carlo"))
conv_fig.add_hline(y=benchmark, line_dash="dash", annotation_text="Black-Scholes")
conv_fig.update_xaxes(type="log", title="Number of Simulations")
conv_fig.update_yaxes(title="Option Price")
conv_fig.update_layout(height=450)
st.plotly_chart(conv_fig, use_container_width=True)

st.subheader("Asian Option Extension")
asian_option = st.selectbox("Asian Option Type", ["Call", "Put"], key="asian_type")
asian_price, asian_se, asian_low, asian_high = monte_carlo_asian(
    S, K, T, r, sigma, simulations, steps, asian_option, int(seed), antithetic
)
ac1, ac2, ac3 = st.columns(3)
ac1.metric("Asian MC Price", f"{asian_price:.4f}")
ac2.metric("Std. Error", f"{asian_se:.6f}")
ac3.metric("95% CI", f"[{asian_low:.4f}, {asian_high:.4f}]")

with st.expander("Methodology"):
    st.markdown("""
    **Geometric Brownian Motion**

    $S_{t+dt}=S_t\exp((r-\frac{1}{2}\sigma^2)dt+\sigma\sqrt{dt}Z)$

    The Monte Carlo estimator discounts the simulated risk-neutral payoff back at the risk-free rate. Antithetic variates use paired random draws $(Z,-Z)$ to reduce variance. The control-variate implementation uses discounted terminal stock value, whose risk-neutral expectation is known exactly as the spot price.
    """)
