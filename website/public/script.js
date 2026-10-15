document.getElementById('year').textContent = new Date().getFullYear();

// Scroll reveal
const io = new IntersectionObserver((entries) => {
  entries.forEach((e) => {
    if (e.isIntersecting) { e.target.classList.add('shown'); io.unobserve(e.target); }
  });
}, { threshold: 0.15 });
document.querySelectorAll('.reveal').forEach((el) => io.observe(el));

// Copy install command
const btn = document.getElementById('copy');
const msg = document.getElementById('copy-msg');
const icoCopy = document.getElementById('ico-copy');
const icoCheck = document.getElementById('ico-check');
btn.addEventListener('click', async () => {
  try {
    await navigator.clipboard.writeText('brew install ethandadev/tap/boro');
    msg.textContent = 'Copied to clipboard.';
    icoCopy.hidden = true; icoCheck.hidden = false;
    setTimeout(() => {
      msg.textContent = 'Click to copy.';
      icoCopy.hidden = false; icoCheck.hidden = true;
    }, 1800);
  } catch (e) { /* clipboard unavailable */ }
});
