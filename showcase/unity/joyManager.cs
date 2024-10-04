using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class joyManager : MonoBehaviour
{
    public int direksiyonValue, gazValue, frenValue, debriyajValue;

    [Space(20)]

    public int direksiyonMaxDerece;

    [SerializeField] TMP_InputField direksiyonDereceField;

    [SerializeField] Slider direksiyonDereceSlider;

    [SerializeField] bool isSlider;

    [Space(20)]

    [SerializeField] TextMeshProUGUI direksiyonAciText;
    [SerializeField] TextMeshProUGUI gazAciText, frenAciText, debriyajAciText;

    [Space(20)]

    [SerializeField] Image gazValueImage;
    [SerializeField] Image frenValueImage, debriyajValueImage;

    [Space(20)]

    [SerializeField] GameObject steeringWheel;

    void Start()
    {
        setSlider();
    }

    void Update()
    {
        axisWrite();

        pedalWrite();

        if (isSlider)
        {
            if(int.TryParse(direksiyonDereceField.text, out int direksiyonDereceValue))
            {
                direksiyonMaxDerece = direksiyonDereceValue;
            }
            
            direksiyonDereceSlider.value = direksiyonMaxDerece;
            isSlider = false;
        }
    }

    void pedalWrite()
    {
        steeringWheel.transform.rotation = Quaternion.Euler(new Vector3(0, 0, -direksiyonValue));

        gazValueImage.fillAmount = (gamepadManager.instance.RxAxis + 1) / 2;
        frenValueImage.fillAmount = (gamepadManager.instance.RyAxis + 1) / 2;
        debriyajValueImage.fillAmount = (gamepadManager.instance.RzAxis + 1) / 2;
    }

    void axisWrite()
    {
        int maxDireksiyonValue = 270;
        int maxPedaValue = 100;
        direksiyonValue = (int)(gamepadManager.instance.zAxis * (maxDireksiyonValue / 2));
        gazValue = (int)((gamepadManager.instance.RxAxis + 1) * (maxPedaValue / 2));
        frenValue = (int)((gamepadManager.instance.RyAxis + 1) * (maxPedaValue / 2));
        debriyajValue = (int)((gamepadManager.instance.RzAxis + 1) * (maxPedaValue / 2));

        direksiyonAciText.text = direksiyonValue.ToString();
        gazAciText.text = gazValue.ToString();
        frenAciText.text = frenValue.ToString();
        debriyajAciText.text = debriyajValue.ToString();
    }

    public void setField()
    {
        isSlider = true;
    }
    public void setSlider()
    {
        direksiyonMaxDerece = (int)(direksiyonDereceSlider.value);
        direksiyonDereceField.text = direksiyonMaxDerece.ToString();
        isSlider = true;
    }
}
