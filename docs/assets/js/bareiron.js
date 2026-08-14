
// BAREIRON static site — vanilla JS helpers
// No frameworks. No analytics. Respects prefers-reduced-motion.

(function () {
  'use strict';

  const reducedMotion = window.matchMedia('(prefers-reduced-motion: reduce)').matches;

  // Copy buttons
  document.querySelectorAll('.copy-btn').forEach(function (btn) {
    btn.addEventListener('click', function () {
      const targetId = btn.getAttribute('data-target');
      const target = document.getElementById(targetId);
      if (!target) return;
      const text = target.innerText;
      navigator.clipboard.writeText(text).then(function () {
        const original = btn.innerText;
        btn.innerText = 'COPIED';
        btn.setAttribute('aria-label', 'Copied');
        window.setTimeout(function () {
          btn.innerText = original;
          btn.setAttribute('aria-label', 'Copy ' + targetId);
        }, 1500);
      }).catch(function () {
        // Fallback for environments without clipboard API
        const area = document.createElement('textarea');
        area.value = text;
        area.style.position = 'fixed';
        area.style.opacity = '0';
        document.body.appendChild(area);
        area.select();
        try {
          document.execCommand('copy');
          btn.innerText = 'COPIED';
        } catch (e) {
          btn.innerText = 'FAIL';
        }
        document.body.removeChild(area);
        window.setTimeout(function () {
          btn.innerText = 'COPY';
        }, 1500);
      });
    });
  });

  // Platform tabs
  const tabList = document.querySelector('.platform-tabs');
  if (tabList) {
    const buttons = tabList.querySelectorAll('.tab-btn');
    const panels = document.querySelectorAll('.tab-panel');

    buttons.forEach(function (btn) {
      btn.addEventListener('click', function () {
        const controls = btn.getAttribute('aria-controls');

        buttons.forEach(function (b) {
          b.classList.remove('active');
          b.setAttribute('aria-selected', 'false');
        });
        panels.forEach(function (p) {
          p.classList.remove('active');
          p.setAttribute('hidden', '');
        });

        btn.classList.add('active');
        btn.setAttribute('aria-selected', 'true');
        const panel = document.getElementById(controls);
        if (panel) {
          panel.classList.add('active');
          panel.removeAttribute('hidden');
        }
      });
    });
  }

  // Subtle flicker on load only if motion is allowed
  if (!reducedMotion) {
    const scanlines = document.querySelector('.scanlines');
    if (scanlines) {
      scanlines.style.opacity = '0.28';
      window.setTimeout(function () {
        scanlines.style.transition = 'opacity 0.25s ease';
        scanlines.style.opacity = '0.22';
      }, 120);
    }
  }
})();
